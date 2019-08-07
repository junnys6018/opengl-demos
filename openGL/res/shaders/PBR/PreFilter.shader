#shader Vertex
#version 330 core
layout(location = 0) in vec3 v_position;

out vec3 f_position;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	f_position = v_position;
	gl_Position = proj * view * vec4(v_position, 1.0);
}

#shader Fragment
#version 330 core

out vec4 FragColor;
in vec3 f_position;

uniform samplerCube environmentMap;
uniform float roughness;
const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);

void main()
{
	// Assume viewDir is the same as the sampled normal
	vec3 normal = normalize(f_position);
	vec3 viewDir = normal;

	const uint SAMPLE_COUNT = 1024u;
	float totalWeight = 0.0;
	vec3 prefilteredColor = vec3(0.0);

	for (uint i = 0u; i < SAMPLE_COUNT; i++)
	{
		vec2 Xi = Hammersley(i, SAMPLE_COUNT);
		vec3 halfDir = ImportanceSampleGGX(Xi, normal, roughness);
		vec3 reflectDir = normalize(2.0 * dot(viewDir, halfDir) * halfDir - viewDir);

		float NdotL = dot(normal, reflectDir);
		if (NdotL > 0.0)
		{
			// sample from the environment's mip level based on roughness/pdf
			float D = DistributionGGX(normal, halfDir, roughness);
			float NdotH = max(dot(normal, halfDir), 0.0);
			float HdotV = max(dot(halfDir, viewDir), 0.0);
			float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

			float resolution = 512.0; // resolution of source cubemap (per face)
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			prefilteredColor += texture(environmentMap, reflectDir).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefilteredColor = prefilteredColor / totalWeight;

	FragColor = vec4(prefilteredColor, 1.0);
}

float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// Returns i-th index of a low discrepency sequence of N samples
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}
// Samples a microfacet halfdir vector, parameterised by roughness
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space vector to world-space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}