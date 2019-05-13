#shader Vertex
#version 430 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_texCoord;

out vec2 f_texCoord;

void main()
{
	f_texCoord = v_texCoord;
	gl_Position = vec4(v_position, 0.0, 1.0);
}

#shader Fragment
#version 430 core
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform int renderMode;
uniform int NUM_LIGHTS;
uniform vec3 camPos;

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
in vec2 f_texCoord;

void main()
{
	color.a = 1.0;
	// Lighting
	if (renderMode == 0)
	{
		vec3 fragPos = texture(gPosition, f_texCoord).rgb;
		vec3 normal = texture(gNormal, f_texCoord).rgb;
		vec3 albedo = texture(gAlbedoSpec, f_texCoord).rgb;
		float specPow = texture(gAlbedoSpec, f_texCoord).a;

		vec3 lightColor = albedo * 0.1;
		for (int i = 0; i < NUM_LIGHTS; i++)
		{
			PointLight p = pointLights[i];

			// Diffuse;
			vec3 lightDir = normalize(fragPos - p.position);
			float diff = max(dot(-lightDir, normal), 0.0);
			// Specular
			vec3 reflectDir = reflect(lightDir, normal);
			vec3 viewDir = normalize(camPos - fragPos);
			vec3 halfDir = normalize(-lightDir + viewDir);
			float spec = pow(max(dot(halfDir, normal), 0.0), specPow);
			// Attenuation
			float distance = length(p.position - fragPos);
			if (distance > p.radius)
				continue;
			float attenuation = 1.0 - distance / p.radius;

			// combine results
			lightColor += attenuation * p.intensity * p.color * (diff + spec) * albedo;
		}
		lightColor = vec3(1.0) - exp(-lightColor * 0.5);
		color = vec4(lightColor, 1.0);
	}
	else if (renderMode == 1)
		color.rgb = texture(gPosition, f_texCoord).rgb;
	else if (renderMode == 2)
		color.rgb = 0.5 * texture(gNormal, f_texCoord).rgb + vec3(0.5);
	else if (renderMode == 3)
		color.rgb = texture(gAlbedoSpec, f_texCoord).rgb;
	else if (renderMode == 4)
		color.rgb = texture(gAlbedoSpec, f_texCoord).aaa;
}
