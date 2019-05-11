#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;
layout(location = 3) in vec4 v_tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 f_normal;
out vec3 f_fragPos;
out vec2 f_texCoord;
out mat3 f_TBN;
void main()
{
	vec3 T = normalize(vec3(model * vec4(v_tangent.xyz, 0.0)));
	vec3 N = normalize(vec3(model * vec4(v_normal, 0.0)));
	vec3 B = v_tangent.w * cross(N, T);
	mat3 TBN = mat3(T, B, N);
	f_TBN = TBN;
	f_normal = normalize(mat3(transpose(inverse(model))) * v_normal);
	f_fragPos = vec3(model * vec4(v_position, 1.0));
	f_texCoord = v_texCoord;

	gl_Position = proj * view * model * vec4(v_position, 1.0);

}

#shader Fragment
#version 330 core

struct PointLight
{
	vec3 position;

	float constant;
	float lin;
	float quadratic;
};

out vec4 color;
uniform vec3 camPos;
uniform PointLight light;

uniform bool useNormMap;
uniform bool visNormMap;

uniform sampler2D Texture1;
uniform sampler2D NormMap;

// Normalised
in vec3 f_normal;
in vec3 f_fragPos;
in vec2 f_texCoord;
in mat3 f_TBN;
void main()
{
	vec3 normal;
	if (useNormMap)
	{
		// normals needed to be normalized from texture for some reason
		normal = normalize(vec3(texture(NormMap, f_texCoord)));
		normal = normalize(normal * 2.0 - 1.0);
		normal = f_TBN * normal;
	}
	else
		normal = f_normal;
	// Diffuse;
	vec3 lightDir = normalize(f_fragPos - light.position);
	float diff = max(dot(-lightDir, normal), 0.0);
	// Specular
	float spec;
	vec3 viewDir = normalize(camPos - f_fragPos);

	vec3 halfDir = normalize(-lightDir + viewDir);
	spec = pow(max(dot(halfDir, normal), 0.0), 32);
	// Attenuation
	float distance = length(light.position - f_fragPos);
	float attenuation = 1.0 / (light.constant + light.lin * distance + light.quadratic * (distance * distance));

	// combine results
	vec3 ambient = 0.3 * vec3(texture(Texture1, f_texCoord));
	vec3 diffuse = 0.9 * diff * vec3(texture(Texture1, f_texCoord));
	vec3 specular = vec3(0.3) * spec;

	if (visNormMap)
		color = vec4(normal * 0.5 + vec3(0.5), 1.0);
		//color = texture(NormMap, f_texCoord);
	else
		color = vec4(attenuation * (ambient + diffuse + specular), 1.0);
	
}