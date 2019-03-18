#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
out vec3 f_color;
uniform mat4 MVP;


void main()
{
	gl_Position = MVP * vec4(v_position, 1.0);
	f_color = v_position;
}

#shader Fragment
#version 330 core

in vec3 f_color;
out vec4 color;


void main()
{
	color = vec4(f_color, 1.0);
}
