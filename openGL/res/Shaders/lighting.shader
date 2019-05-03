#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoords;

uniform mat4 MVP;
uniform mat4 MV;

out vec3 s_normal;
out vec3 s_fragPos;
out vec2 s_texCoords;

void main()
{
	s_normal = mat3(transpose(inverse(MV))) * v_normal;
	s_fragPos = vec3(MV * vec4(v_position, 1.0));
	s_texCoords = v_texCoords;

	gl_Position = MVP * vec4(v_position, 1.0);
}

#shader Fragment
#version 330 core

out vec4 fragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	int shininess;
};
struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float lin;
	float quadratic;
};
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct SpotLight
{
	float cutOff;	// cos(cutoffangle)
	float outerCutOff;
	vec3 color;

	float constant;
	float lin;
	float quadratic;

	bool isOn;
};
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light);

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material material;
uniform bool useEmission;

in vec3 s_normal;
in vec3 s_fragPos;
in vec2 s_texCoords;
// lighting calculations done in VIEWSPACE
void main()	
{
	vec3 norm = normalize(s_normal);
	vec3 viewDir = normalize(s_fragPos);

	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	// phase 2: Point lights
	for (int i = 0; i != NR_POINT_LIGHTS; ++i)
		result += CalcPointLight(pointLights[i], s_normal, s_fragPos, viewDir);
	// phase 3: Emission maps
	if (useEmission)
		result += texture(material.emission, s_texCoords).rgb;
	// phase 4: Spot lighting
	result += CalcSpotLight(spotLight);
	fragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(-viewDir, reflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse, s_texCoords));
	vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse, s_texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, s_texCoords));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(-viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.lin * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, s_texCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, s_texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, s_texCoords));
	
	return attenuation * (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light)
{
	// in view space look dir is always (0.0, 0.0, -1.0)
	if (!light.isOn)
		return vec3(0.0);
	float theta = dot(vec3(0.0, 0.0, -1.0), normalize(s_fragPos));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	float distance = length(s_fragPos);
	float attenuation = 1.0 / (light.constant + light.lin * distance + light.quadratic * (distance * distance));

	return attenuation * intensity * light.color;
}



