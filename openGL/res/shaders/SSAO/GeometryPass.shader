#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// view space position and normal vectors
out vec3 f_fragPos;
out vec2 f_texCoord;
out vec3 f_normal;

void main()
{
	vec4 viewPos = view * model * vec4(v_position, 1.0);
	f_fragPos = viewPos.xyz;
	f_texCoord = v_texCoord;
	f_normal = mat3(transpose(inverse(view * model))) * v_normal;

	gl_Position = proj * viewPos;
}

#shader Fragment
#version 420 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gAlbedo;

in vec3 f_fragPos;
in vec2 f_texCoord;
in vec3 f_normal;

uniform sampler2D Texture1;

void main()
{
	gPosition = f_fragPos;
	gNormal = normalize(f_normal);
	gAlbedo = texture(Texture1, f_texCoord).rgb;
}