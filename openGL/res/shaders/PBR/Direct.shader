#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoords;

uniform mat4 VP;
uniform mat4 model;

out vec3 f_normal;
out vec3 f_fragPos;
out vec2 f_texCoords;

void main()
{
	f_normal = mat3(transpose(inverse(model))) * v_normal;
	f_fragPos = vec3(model * vec4(v_position, 1.0));
	f_texCoords = v_texCoords;

	gl_Position = VP * model * vec4(v_position, 1.0);
}

#shader Fragment
#version 330 core

out vec4 fragColor;

in vec3 f_normal;
in vec3 f_fragPos;
in vec2 f_texCoords;

uniform float metalness;
uniform float roughness;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2D Metalness;
uniform sampler2D Roughness;

uniform vec3 lightPos[4];
uniform vec3 lightColor[4];

uniform vec3 viewPos;
uniform bool useSamplers;
uniform int renderMode;

const float PI = 3.14159265359;
vec3 freshnelSchlick(float HdotV, vec3 F0);
float DistributionGGX(float NdotH, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 getNormalFromMap();
void main()
{
	vec3 albedo;
	vec3 normal;
	float metal;
	float rough;
	if (useSamplers)
	{
		albedo = pow(texture(Albedo, f_texCoords).rgb, vec3(2.2));
		normal = getNormalFromMap();
		metal = texture(Metalness, f_texCoords).r;
		rough = texture(Roughness, f_texCoords).r;
	}
	else
	{
		albedo = vec3(1.0, 0.0, 0.0);
		normal = normalize(f_normal);
		metal = metalness;
		rough = roughness;
	}
	vec3 viewDir = normalize(viewPos - f_fragPos);

	vec3 Irradiance = vec3(0.0);
	for (int i = 0; i < 4; i++) // loop over intergral
	{
		vec3 lightDir = normalize(lightPos[i] - f_fragPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		float NdotL = max(dot(normal, lightDir), 0.0);

		float distance = length(lightPos[i] - f_fragPos);
		float attenuation = 1.0 / (distance * distance);

		vec3 radiance = attenuation * lightColor[i];
		// Cook-Torrance specular BRDF term
		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedo, metal);
		vec3 F = freshnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);
		float D = DistributionGGX(max(dot(normal, halfDir), 0.0), rough);
		float G = GeometrySmith(normal, viewDir, lightDir, rough);

		vec3 numerator = D * F * G;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL;

		vec3 specular = numerator / max(denominator, 0.001);
		// Cook-Torrance diffuse BRDF term
		vec3 Kd = vec3(1.0) - F;
		Kd *= 1.0 - metal;

		if (renderMode == 0) // Lighting
		{
			Irradiance += (Kd * albedo / PI + specular) * radiance * NdotL;
		}
		else if (renderMode == 1) // Diffuse
		{
			Irradiance += Kd * albedo / PI * radiance * NdotL;
		}
		else if (renderMode == 2) // Specular
		{
			Irradiance += specular * radiance * NdotL;
		}
		else if (renderMode == 3) // Distribution
		{
			Irradiance += vec3(D);
		}
		else if (renderMode == 4) // Freshnel
		{
			Irradiance += F;
		}
		else if (renderMode == 5) // Geometry
		{
			Irradiance += vec3(G);
		}
	}
	if (renderMode == 0 || renderMode == 1 || renderMode == 2)
	{
		Irradiance = 0.03 * albedo + Irradiance;
	}
	// Gamma correct
	Irradiance = Irradiance / (Irradiance + vec3(1.0));
	Irradiance = pow(Irradiance, vec3(1.0 / 2.2));

	fragColor = vec4(Irradiance, 1.0);
}

vec3 freshnelSchlick(float HdotV, vec3 F0)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - HdotV, 5.0);
}

float DistributionGGX(float NdotH, float r)
{
	float r2 = r * r;
	float NdotH2 = NdotH * NdotH;
	float denom = PI * pow(NdotH2 * (r2 - 1.0) + 1.0, 2.0);

	return r2 / denom;
}

float GeometrySchlickGGX(float NdotV, float r)
{
	float R = (r + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float r)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, r);
	float ggx1 = GeometrySchlickGGX(NdotL, r);

	return ggx1 * ggx2;
}
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(Normal, f_texCoords).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(f_fragPos);
	vec3 Q2 = dFdy(f_fragPos);
	vec2 st1 = dFdx(f_texCoords);
	vec2 st2 = dFdy(f_texCoords);

	vec3 N = normalize(f_normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}