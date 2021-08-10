#shader Vertex
#version 430 core

layout(location = 0) in vec3 v_position;
layout(location = 3) in mat4 v_model;
struct PointLight
{
	vec3 position;
	vec3 color;
	float intensity;
	float radius;
};
layout(std140, binding = 0) buffer BufferObject
{
	PointLight pointLights[];
};
layout(std140, binding = 1) uniform Matrix
{
	mat4 view;
	mat4 proj;
};

out vec3 color;

void main()
{
	gl_Position = proj * view * v_model * vec4(v_position, 1.0);
	color = pointLights[gl_InstanceID].color;
}

#shader Fragment
#version 430 core
out vec4 fragColor;

in vec3 color;

void main()
{
	fragColor = vec4(color, 1.0);
}