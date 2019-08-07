#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoords;
layout(location = 3) in vec4 v_tangent;

uniform mat4 VP;
uniform mat4 model;

out vec3 f_normal;
out vec3 f_fragPos;
out vec2 f_texCoords;
out mat3 TBN;

void main()
{
	vec3 T = normalize(vec3(model * vec4(v_tangent.xyz, 0.0)));
	vec3 N = normalize(vec3(model * vec4(v_normal, 0.0)));
	vec3 B = v_tangent.w * cross(N, T);
	TBN = mat3(T, B, N);

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
in mat3 TBN;

uniform float metalness;
uniform float roughness;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2D Metalness;
uniform sampler2D Roughness;
uniform samplerCube irradianceMap;

uniform vec3 viewPos;
uniform bool useSamplers;
uniform int renderMode;

const float PI = 3.14159265359;

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
void main()
{
	vec3 albedo;
	vec3 normal;
	float metal;
	float rough;
	if (useSamplers)
	{
		albedo = pow(texture(Albedo, f_texCoords).rgb, vec3(2.2));
		normal = normalize(TBN * (texture(Normal, f_texCoords).xyz * 2.0 - 1.0));
		metal = texture(Metalness, f_texCoords).r;
		rough = texture(Roughness, f_texCoords).r;
	}
	else
	{
		albedo = vec3(1.00, 0.71, 0.29); // F0 for gold
		normal = normalize(f_normal);
		metal = metalness;
		rough = roughness;
	}
	vec3 irradiance = texture(irradianceMap, normal).rgb;
	vec3 viewDir = normalize(viewPos - f_fragPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metal);
	vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, rough);
	vec3 Kd = 1.0 - F;
	Kd *= 1.0 - metal;

	vec3 color = Kd * albedo * irradiance;
	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));
	if (renderMode == 1) // Lighting
	{
		fragColor = vec4(color, 1.0);
	}
	else if (renderMode == 2) // Freshnel
	{
		fragColor = vec4(F, 1.0);
	}
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}