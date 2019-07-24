#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
uniform mat4 model;
uniform mat4 lightTransform;

void main()
{
	gl_Position = lightTransform * model * vec4(v_position, 1.0);
}

#shader Geometry
#version 420 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 projMatrices[6];

out vec4 g_position;

void main()
{
	for (int face = 0; face < 6; face++)
	{
		gl_Layer = face; // built-in variable that specifies to which face we render.
		for (int i = 0; i < 3; i++)
		{
			g_position = gl_in[i].gl_Position;
			gl_Position = projMatrices[face] * g_position;
			EmitVertex();
		}
		EndPrimitive();
	}
}

#shader Fragment
#version 420 core

in vec4 g_position;
uniform float far_plane;

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(g_position.xyz);

	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance / far_plane;

	// write this as modified depth
	gl_FragDepth = lightDistance;
}
