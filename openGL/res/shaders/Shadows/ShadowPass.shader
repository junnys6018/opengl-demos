#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
	mat4 lightSpace;
};;


void main()
{
	gl_Position = lightSpace * model * vec4(v_position, 1.0);
}

#shader Fragment
#version 420 core

void main()
{

}
