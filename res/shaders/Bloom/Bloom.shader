#shader Vertex
#version 420

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
	mat4 view;
	mat4 proj;
};

out VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} vs;

void main()
{
	vs.fragPos = vec3(model * vec4(v_position, 1.0));
	vs.normal = normalize(mat3(transpose(inverse(model))) * v_normal);
	vs.texCoord = v_texCoord;

	gl_Position = proj * view * model * vec4(v_position, 1.0);
}

#shader Fragment
#version 420

uniform sampler2D Texture1;
layout(std140, binding = 1) uniform lights
{
	vec3[4] positions;
	vec3[4] colors;
};
in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} vs;
uniform vec3 viewPos;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

vec3 BlinnPhong(vec3 position, vec3 color);
void main()
{
	if (texture(Texture1, vs.texCoord).a < 0.1)
		discard;
	FragColor = vec4(0.0);
	// For each pointlight
	for (int i = 0; i < 4; i++)
	{
		FragColor += vec4(BlinnPhong(positions[i], colors[i]), 1.0);
	}
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 4.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

vec3 BlinnPhong(vec3 position, vec3 color)
{
	// Diffuse;
	vec3 lightDir = normalize(vs.fragPos - position);
	float diff = max(dot(-lightDir, vs.normal), 0.0);
	// Specular
	float spec;
	vec3 reflectDir = reflect(lightDir, vs.normal);
	vec3 viewDir = normalize(viewPos - vs.fragPos);
	vec3 halfDir = normalize(-lightDir + viewDir);
	spec = pow(max(dot(halfDir, vs.normal), 0.0), 24);
	// Attenuation
	float distance = length(position - vs.fragPos);
	float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));

	// combine results
	return attenuation * color * (0.1 + 0.7 * diff + 0.1 * spec) * vec3(texture(Texture1, vs.texCoord));
}
