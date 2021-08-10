#shader Vertex
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_TexCoord;

out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(v_position, 0.0, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader Fragment
#version 330 core

out vec4 FragColor;
in vec2 f_TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D SSAO;

uniform int renderMode;

struct Light {
	vec3 Position;
	vec3 Color;
};
uniform Light light;

void main()
{
	vec3 fragPos = texture(gPosition, f_TexCoord).rgb;
	vec3 normal = texture(gNormal, f_TexCoord).rgb;
	vec3 diffuse = texture(gAlbedo, f_TexCoord).rgb;
	float ambientOcclusion = texture(SSAO, f_TexCoord).r;
	if (renderMode == 0)
	{
		FragColor = vec4(fragPos, 1.0);
	}
	else if (renderMode == 1)
	{
		FragColor = vec4(normal, 1.0);
	}
	else if (renderMode == 2)
	{
		FragColor = vec4(diffuse, 1.0);
	}
	else if (renderMode == 3 || renderMode == 4)
	{
		FragColor = vec4(vec3(ambientOcclusion), 1.0);
	}
	else if (renderMode == 5 || renderMode == 6)
	{
		if (renderMode == 6)
		{
			// No SSAO
			ambientOcclusion = 0.42;
		}
		// Diffuse
		vec3 lightDir = normalize(fragPos - light.Position);
		float diff = max(dot(-lightDir, normal), 0.0);
		// Specular
		vec3 viewDir = normalize(-fragPos);
		vec3 halfDir = normalize(-lightDir + viewDir);
		float spec = pow(max(dot(halfDir, normal), 0.0), 8.0);
		// Attenuation
		float distance = length(light.Position - fragPos);
		float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

		FragColor = vec4((attenuation * (diff + spec) * light.Color + 0.27 * ambientOcclusion) * diffuse, 1.0);
	}
}