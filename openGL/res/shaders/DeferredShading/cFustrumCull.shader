#shader Compute
#version 430

#define THREAD_COUNT 240 // 16 * 15
#define MAX_LIGHTS 40 // max lights in each fustra
#define NEAR 0.1
#define FAR 100.0

layout(local_size_x = 16, local_size_y = 15) in;
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

uniform int renderMode;
uniform int NUM_LIGHTS;
uniform vec2 resolution;
uniform vec3 camPos;
uniform float exposure;
uniform bool visualiseLights;

shared uint pointLightIndex[MAX_LIGHTS];
shared uint pointLightCount = 0;
shared uint minDepth = 0xFFFFFFFF;
shared uint maxDepth = 0;

void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 sampleCoord = pixelCoord / resolution;

	//if (gl_LocalInvocationID.x == 0 || gl_LocalInvocationID.y == 0 || gl_LocalInvocationID.x == 16 || gl_LocalInvocationID.y == 16)
	//	imageStore(img_output, pixelCoord, vec4(0.2, 0.2, 0.2, 1.0f));
	/*else*/ if (renderMode == 0)
	{
		vec3 fragPos = texture(gPosition, sampleCoord).rgb;
		float depthf = vec3(view * vec4(fragPos, 1.0)).z;
		//if (depthf < FAR && depthf > NEAR)
		//{
			// Avoid shading skybox/background or otherwise invalid pixels
			uint depth = uint(depthf * 0xFFFFFFFF);
			atomicMin(minDepth, depth);
			atomicMax(maxDepth, depth);
		//}

		barrier();
		float minDepthZ = float(minDepth / float(0xFFFFFFFF));
		float maxDepthZ = float(maxDepth / float(0xFFFFFFFF));
		vec2 workGroupSize = vec2(16.0, 15.0);
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
		frustumPlanes[4] = vec4(0.0f, 0.0f, 1.0f, -minDepthZ);
		frustumPlanes[5] = vec4(0.0f, 0.0f, -1.0f, maxDepthZ);

		for (int i = 0; i < 4; i++)
		{
			frustumPlanes[i] *= 1.0 / length(frustumPlanes[i].xyz);
		}
		barrier();
		// Culling 
		int passCount = (NUM_LIGHTS + THREAD_COUNT - 1) / THREAD_COUNT;
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
		if (visualiseLights)
		{
			if (gl_LocalInvocationID.x == 0 || gl_LocalInvocationID.y == 0 || gl_LocalInvocationID.x == 16 || gl_LocalInvocationID.y == 16)
				imageStore(img_output, pixelCoord, vec4(.2f, .2f, .2f, 1.0f));
			else
			{
				float lightDensity = pointLightCount / float(MAX_LIGHTS);
				imageStore(img_output, pixelCoord, vec4(vec3(lightDensity), 1.0));
			}
		}
		else
		{
			vec3 normal = texture(gNormal, sampleCoord).rgb;
			vec3 albedo = texture(gAlbedoSpec, sampleCoord).rgb;
			float specFactor = texture(gAlbedoSpec, sampleCoord).a;

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
				float attenuation = 1.0 - distance / p.radius;

				// combine results
				lightColor += attenuation * p.intensity * p.color * (diff + specFactor * spec) * albedo;
			}
			lightColor = vec3(1.0) - exp(-lightColor * exposure);
			imageStore(img_output, pixelCoord, vec4(lightColor, 1.0));
		}
	}

	else if (renderMode == 1)
	{
		imageStore(img_output, pixelCoord, vec4(texture(gPosition, sampleCoord).rgb, 1.0));
	}
	else if (renderMode == 2)
	{
		vec3 color = 0.5 * texture(gNormal, sampleCoord).rgb + vec3(0.5);
		imageStore(img_output, pixelCoord, vec4(color, 1.0));
	}
	else if (renderMode == 3)
	{
		imageStore(img_output, pixelCoord, vec4(texture(gAlbedoSpec, sampleCoord).rgb, 1.0));
	}
	else if (renderMode == 4)
	{
		imageStore(img_output, pixelCoord, vec4(texture(gAlbedoSpec, sampleCoord).aaa, 1.0));
	}
}