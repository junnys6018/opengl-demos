#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(v_position, 1.0);
}

#shader Fragment
#version 330 core

out vec4 color;

void main()
{
	color = vec4(0.04, 0.28, 0.26, 1.0);
}
