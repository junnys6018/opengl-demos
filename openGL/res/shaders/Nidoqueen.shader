#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 2) in vec2 v_texCoord;
uniform mat4 MVP;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
	mat4 view;
	mat4 proj;
};

out vec2 f_texCoord;

void main()
{
	gl_Position = proj * view * model * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}

#shader Fragment
#version 420 core

in vec2 f_texCoord;
uniform sampler2D Texture1;

out vec4 color;


void main()
{
	color = texture(Texture1, f_texCoord);
}
