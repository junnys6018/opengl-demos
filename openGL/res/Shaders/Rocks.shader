#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 2) in vec2 v_texCoord;
layout(location = 3) in mat4 v_model;

out vec2 f_texCoord;
uniform mat4 VP;

void main()
{
	gl_Position = VP * v_model * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}


#shader Fragment
#version 330 core

uniform sampler2D Texture1;
in vec2 f_texCoord;
out vec4 color;

void main()
{
	color = texture(Texture1, f_texCoord);
}