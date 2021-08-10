#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;
uniform mat4 model;
uniform mat4 VP;

out vec3 f_position;
out vec3 f_normal;
out vec2 f_texCoord;

void main()
{
	gl_Position = VP * model * vec4(v_position, 1.0);

	f_position = vec3(model * vec4(v_position, 1.0));
	f_normal = inverse(transpose(mat3(model))) * v_normal;
	f_texCoord = v_texCoord;
}

#shader Fragment
#version 420 core

in vec3 f_position;
in vec3 f_normal;
in vec2 f_texCoord;

uniform sampler2D Texture1;
uniform samplerCube depthMap;
uniform bool renderDepthMap;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float far_plane;

vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(0, 0, 0), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

out vec4 color;
void main()
{
	vec3 albedo = texture(Texture1, f_texCoord).rgb;
	vec3 normal = normalize(f_normal);
	// Diffuse
	vec3 lightDir = f_position - lightPos;
	float diff = max(dot(lightDir, normal), 0.0);
	// Specular
	vec3 viewDir = f_position - viewPos;
	vec3 halfDir = normalize(-lightDir - viewDir);
	float spec = pow(max(dot(halfDir, normal), 0.0), 12.0);
	// Attenuation
	float dist = length(lightDir);
	float attenuation = 1.0 / (1.0 + 0.14 * dist + 0.07 * dist * dist);
	// Shadow
	vec3 fragToLight = f_position - lightPos;
	float shadow = 0.0;
	float bias = max(0.1 * (1.0 - dot(f_normal, lightDir)), 0.01);
	int samples = 20;
	float diskRadius = 0.05f;
	float currentDepth = length(fragToLight);

	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;
		shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
	}
	shadow /= float(samples);

	if (renderDepthMap)
	{
		float closestDepth = texture(depthMap, fragToLight).r;
		color = vec4(vec3(closestDepth), 1.0);
	}
	else
		color = vec4((0.3 + attenuation * (1 - shadow) * (diff + spec) * lightColor) * albedo, 1.0);
}