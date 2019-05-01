#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_TexCoord;
uniform mat4 VP;
uniform mat4 model;
layout(std140, binding = 0) uniform matrix
{
	mat4 lightSpace;
};;

out vec3 f_position;
out vec4 f_lightSpacePos;
out vec3 f_normal;
out vec2 f_TexCoord;

void main()
{
	f_position = vec3(model * vec4(v_position, 1.0));
	f_lightSpacePos = lightSpace * model * vec4(v_position, 1.0);
	f_normal = normalize(vec3(transpose(inverse(model)) * vec4(v_normal, 0.0)));
	f_TexCoord = v_TexCoord;

	gl_Position = VP * model * vec4(v_position, 1.0);
}

#shader Fragment
#version 420 core

uniform sampler2D Texture1;
uniform sampler2D depthMap;

uniform vec3 lightDir;
uniform vec3 viewPos;

in vec3 f_position;
in vec4 f_lightSpacePos;
in vec3 f_normal;
in vec2 f_TexCoord;

out vec4 color;

float inShadow()
{
	// perform perspective divide
	vec3 projCoords = f_lightSpacePos.xyz / f_lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	if (projCoords.z > 1.0)
		return 0.0;
	float shadow = 0.0;
	float bias = max(0.004 * (1.0 - dot(f_normal, lightDir)), 0.001);
	float currentDepth = projCoords.z;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	return shadow / 9.0;
}

void main()
{
	// Diffuse
	float diff = max(dot(-lightDir, f_normal), 0.0);
	// Specular
	vec3 viewDir = normalize(viewPos - f_position);
	vec3 halfDir = normalize(-lightDir + viewDir);
	float spec = pow(max(dot(halfDir, f_normal), 0.0), 32);
	// combine results
	vec3 ambient = 0.15 * vec3(texture(Texture1, f_TexCoord));
	vec3 diffuse = 0.9 * diff * vec3(texture(Texture1, f_TexCoord));
	vec3 specular = 0.9 * spec * vec3(texture(Texture1, f_TexCoord));
	color = vec4(ambient + (1.0 - inShadow()) * (diffuse + specular), 1.0);
}
