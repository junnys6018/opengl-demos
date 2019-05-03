#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;

uniform mat4 model;
layout(std140, binding = 0) uniform matrix
{
	mat4 view;
	mat4 proj;
};

void main()
{
	gl_Position = proj * view * model * vec4(v_position, 1.0);
}

#shader Fragment
#version 420 core

out vec4 color;
uniform vec2 screenSize;

void main()
{
	if (gl_FragCoord.x > screenSize.x / 2.0 && gl_FragCoord.y > screenSize.y / 2 ||
		gl_FragCoord.x < screenSize.x / 2.0 && gl_FragCoord.y < screenSize.y / 2)
		color = vec4(1.0, 0.0, 0.0, 1.0);
	else
		color = vec4(0.0, 1.0, 0.0, 1.0);
}

