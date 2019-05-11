#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
	mat4 view;
	mat4 proj;
};

void main()
{
	gl_Position = proj * view * model * vec4(v_position, 1.0);
	gl_PointSize = 3.0 * gl_Position.z;
}

#shader Fragment
#version 420 core

out vec4 color;

void main()
{
	color = vec4(0.7, 0.1, 0.1, 1.0);
}

