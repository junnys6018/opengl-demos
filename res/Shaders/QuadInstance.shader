#shader Vertex
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec2 v_offset;

out vec3 f_color;

void main()
{
	gl_Position = vec4(gl_InstanceID / 100.0 * v_position + v_offset, 0.0, 1.0);
	f_color = v_color;
}


#shader Fragment
#version 330 core

in vec3 f_color;
out vec4 color;

void main()
{
	color = vec4(f_color, 1.0);
}