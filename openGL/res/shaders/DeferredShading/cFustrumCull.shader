#shader Compute
#version 430

#define THREAD_COUNT 256 // 16 * 16
#define MAX_LIGHTS 64 // max lights in each fustra
#define NEAR 0.1
#define FAR 100.0

layout(local_size_x = 16, local_size_y = 16) in;
struct PointLight
{
	vec3 position;
	vec3 color;
	float intensity;
	float radius;
};
layout(std140, binding = 0) buffer BufferObject
{
	PointLight pointLights[];
};
layout(std140, binding = 1) uniform Matrix
{
	mat4 view;
	mat4 proj;
};
layout(rgba16f, binding = 0) uniform image2D img_output;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform int NUM_LIGHTS;
uniform vec2 resolution;
uniform vec3 camPos;
uniform float exposure;
uniform int outTarget;

shared uint pointLightIndex[MAX_LIGHTS];
shared uint pointLightCount;
shared uint minDepth;
shared uint maxDepth;
vec3 GreyScale2RGB(float t);
void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 sampleCoord = pixelCoord / resolution;

	// initalise shared variables
	if (gl_LocalInvocationIndex == 0)
	{
		pointLightCount = 0;
		minDepth = 0xFFFFFFFFu;
		maxDepth = 0;
	}
	barrier();

	vec3 fragPos = texture(gPosition, sampleCoord).rgb;
	float depthf = -(view * vec4(fragPos, 1.0)).z;
	if (depthf < FAR && depthf > NEAR)
	{
		// map [NEAR, FAR] -> [0, 1] linearly
		depthf = (depthf - NEAR) / (FAR - NEAR);
		uint depth = uint(depthf * float(0xFFFFFFFFu));
		atomicMin(minDepth, depth);
		atomicMax(maxDepth, depth);
	}

	barrier();
	vec2 workGroupSize = vec2(16.0, 16.0);
	// Construct fustrum
	vec2 tileScale = resolution / (2.0 * workGroupSize);
	vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

	vec4 c1 = vec4(-proj[0][0] * tileScale.x, 0.0, tileBias.x, 0.0);
	vec4 c2 = vec4(0.0, -proj[1][1] * tileScale.y, tileBias.y, 0.0);
	vec4 c4 = vec4(0.0, 0.0, 1.0, 0.0);

	// Derive frustum planes
	vec4 frustumPlanes[6];
	// Sides
	//right
	frustumPlanes[0] = c4 - c1;
	//left
	frustumPlanes[1] = c4 + c1;
	//bottom
	frustumPlanes[2] = c4 - c2;
	//top
	frustumPlanes[3] = c4 + c2;
	// Near/far
	float minDepthZ = float(minDepth) / float(0xFFFFFFFFu);
	float maxDepthZ = float(maxDepth) / float(0xFFFFFFFFu);
	// map [0, 1] -> [NEAR, FAR] linearly
	minDepthZ = minDepthZ * (FAR - NEAR) + NEAR;
	maxDepthZ = maxDepthZ * (FAR - NEAR) + NEAR;
	frustumPlanes[4] = -vec4(0.0f, 0.0f, 1.0f, minDepthZ);
	frustumPlanes[5] = vec4(0.0f, 0.0f, 1.0f, maxDepthZ);

	for (int i = 0; i < 4; i++)
	{
		frustumPlanes[i] *= 1.0 / length(frustumPlanes[i].xyz);
	}
	barrier();
	// Culling 
	int passCount = (NUM_LIGHTS + THREAD_COUNT - 1) / THREAD_COUNT; // ceil(NUM_LIGHTS / THREAD_COUNT)
	for (int i = 0; i < passCount; i++)
	{
		uint lightIndex = i * THREAD_COUNT + gl_LocalInvocationIndex;
		lightIndex = min(lightIndex, NUM_LIGHTS);

		PointLight p = pointLights[lightIndex];
		vec4 pos = view * vec4(p.position.xyz, 1.0);
		if (pointLightCount < MAX_LIGHTS)
		{
			bool inFustrum = true;
			for (int j = 0; j < 6 && inFustrum; j++)
			{
				float dist = dot(frustumPlanes[j], pos);
				inFustrum = (-p.radius <= dist);
			}

			if (inFustrum)
			{
				uint id = atomicAdd(pointLightCount, 1);
				pointLightIndex[id] = lightIndex;
			}
		}
	}
	barrier();
	// Lighting
	vec3 albedo = texture(gAlbedoSpec, sampleCoord).rgb;
	if (outTarget == 0)
	{
		vec3 normal = texture(gNormal, sampleCoord).rgb;
		float specFactor = texture(gAlbedoSpec, sampleCoord).a;
		// Ambient
		vec3 lightColor = albedo * 0.1;
		vec3 viewDir = normalize(camPos - fragPos);
		for (int i = 0; i < pointLightCount; i++)
		{
			PointLight p = pointLights[pointLightIndex[i]];

			// Diffuse;
			vec3 lightDir = normalize(fragPos - p.position);
			float diff = max(dot(-lightDir, normal), 0.0);
			// Specular
			vec3 reflectDir = reflect(lightDir, normal);
			vec3 halfDir = normalize(-lightDir + viewDir);
			float spec = pow(max(dot(halfDir, normal), 0.0), 24);
			// Attenuation
			float distance = length(p.position - fragPos);
			float attenuation = max(1.0 - distance / p.radius, 0.0);

			// combine results
			lightColor += attenuation * p.intensity * p.color * (diff + specFactor * spec);
		}
		lightColor *= albedo; // albedo can be factored out of summation
		// Tone Mapping
		lightColor = vec3(1.0) - exp(-lightColor * exposure);
		imageStore(img_output, pixelCoord, vec4(lightColor, 1.0));
	}
	else if (outTarget == 1) // Visualize lights
	{
		if (gl_LocalInvocationID.x == 0 || gl_LocalInvocationID.y == 0 || gl_LocalInvocationID.x == 16 || gl_LocalInvocationID.y == 16)
			imageStore(img_output, pixelCoord, vec4(0.2, 0.2, 0.2, 1.0));
		else
		{
			float lightDensity = min(float(pointLightCount) / 10, 1.0);
			vec3 intensity = GreyScale2RGB(lightDensity);

			imageStore(img_output, pixelCoord, vec4(intensity * albedo, 1.0));
		}
	}
	else if (outTarget == 2) // Depth buffer
	{
		imageStore(img_output, pixelCoord, vec4(vec3(float(minDepth) / float(0xFFFFFFFFu)), 1.0));
	}
}

vec3 GreyScale2RGB(float t)
{
	if (0.0 <= t && t < 0.25)
	{
		return vec3(0.0, mix(0.0, 1.0, 4.0 * t), 1.0);
	}
	if (0.25 <= t && t < 0.5)
	{
		return vec3(0.0, 1.0, mix(1.0, 0.0, 4.0 * t - 1.0));
	}
	if (0.5 <= t && t < 0.75)
	{
		return vec3(mix(0.0, 1.0, 4.0 * t - 2.0), 1.0, 0.0);
	}
	if (0.75 <= t && t <= 1.0)
	{
		return vec3(1, mix(1.0, 0.0, 4.0 * t - 3.0), 0.0);
	}
}