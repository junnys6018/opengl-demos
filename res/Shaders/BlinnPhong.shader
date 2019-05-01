#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;

uniform mat4 model;
layout(std140, binding = 0) uniform matrix
{
	mat4 view;
	mat4 proj;
};

out vec3 f_normal;
out vec3 f_fragPos;
out vec2 f_texCoord;
void main()
{
	f_normal = normalize(mat3(transpose(inverse(view * model))) * v_normal);
	f_fragPos = vec3(view * model * vec4(v_position, 1.0));
	f_texCoord = v_texCoord;

	gl_Position = proj * view * model * vec4(v_position, 1.0);

}

#shader Fragment
#version 420 core

struct PointLight
{
	vec3 position;

	float constant;
	float lin;
	float quadratic;
};
struct Config
{
	vec2 screenSize;
	int shadingMode;
	int useGamma;
	float shininess;
};

out vec4 color;

uniform Config config;
uniform PointLight light;
uniform sampler2D Texture1;

// Normalised
in vec3 f_normal;
in vec3 f_fragPos;
in vec2 f_texCoord;
void main()
{
	// Diffuse;
	vec3 lightDir = normalize(f_fragPos - light.position);
	float diff = max(dot(-lightDir, f_normal), 0.0);
	// Specular
	float specBlinn, specPhong;
	vec3 reflectDir = reflect(lightDir, f_normal);
	vec3 viewDir = normalize(-f_fragPos);	
	specPhong = pow(max(dot(reflectDir, viewDir), 0.0), config.shininess / 2.0);

	vec3 halfDir = normalize(-lightDir + viewDir);
	specBlinn = pow(max(dot(halfDir, f_normal), 0.0), config.shininess);
	// Attenuation
	float distance = length(light.position - f_fragPos);
	float attenuation = 1.0 / (light.constant + light.lin * distance + light.quadratic * (distance * distance));
	
	// combine results
	vec3 ambient = 0.1 * vec3(texture(Texture1, f_texCoord));
	vec3 diffuse = 0.7 * diff * vec3(texture(Texture1, f_texCoord));
	vec3 specularPhong = 0.7 * specPhong * vec3(texture(Texture1, f_texCoord));
	vec3 specularBlinn = 0.7 * specBlinn * vec3(texture(Texture1, f_texCoord));

	// PHONG
	if (config.shadingMode == 0)
		color = vec4(attenuation * (ambient + diffuse + specularPhong), 1.0);
	// BLINN_PHONG
	else if (config.shadingMode == 1)
		color = vec4(attenuation * (ambient + diffuse + specularBlinn), 1.0);
	// BOTH
	else if (config.shadingMode == 2)
	{
		if (gl_FragCoord.y > config.screenSize.y / 2.0)
			color = vec4(attenuation * (ambient + diffuse + specularPhong), 1.0);
		else
			color = vec4(attenuation * (ambient + diffuse + specularBlinn), 1.0);
	}
	// Gamma
	if (config.useGamma == 1)
		color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
	// Both
	else if (config.useGamma == 2 && gl_FragCoord.x > config.screenSize.x / 2)
	{
			color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
	}
}