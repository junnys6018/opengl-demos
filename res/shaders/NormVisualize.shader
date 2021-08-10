#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
	mat4 view;
	mat4 proj;
};

out vec4 f_normal;

void main()
{
	gl_Position = proj * view * model * vec4(v_position, 1.0);
	mat4 normalMatrix = transpose(inverse(view * model));
	f_normal = normalize(proj * normalMatrix * vec4(v_normal, 0.0));
}

#shader Geometry
#version 420 core

#define MAGNITUDE 0.08

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec4 f_normal[];

void main()
{
	for (int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = gl_in[i].gl_Position + MAGNITUDE * f_normal[i];
		EmitVertex();

		EndPrimitive();
	}
}
#shader Fragment
#version 420 core

out vec4 color;

void main()
{
	color = vec4(1.0, 1.0, 0.0, 1.0);
}