#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;

uniform mat4 model;
layout(std140, binding = 1) uniform Matrix
{
	mat4 view;
	mat4 proj;
};

out vec3 f_fragPos;
out vec2 f_texCoord;
out vec3 f_normal;

void main()
{
	vec4 worldPos = model * vec4(v_position, 1.0);
	f_fragPos = worldPos.xyz;

	f_texCoord = v_texCoord;

	f_normal = mat3(transpose(inverse(model))) * v_normal;

	gl_Position = proj * view * worldPos;
}

#shader Fragment
#version 420 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 f_fragPos;
in vec2 f_texCoord;
in vec3 f_normal;

uniform sampler2D Texture1;
uniform sampler2D SpecMap;

void main()
{
	if (texture(Texture1, f_texCoord).a < 0.1)
		discard;
	gPosition = f_fragPos;
	gNormal = normalize(f_normal);
	gAlbedoSpec.rgb = texture(Texture1, f_texCoord).rgb;
	gAlbedoSpec.a = texture(SpecMap, f_texCoord).r;
}
