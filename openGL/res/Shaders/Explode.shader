#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;
layout(location = 2) in vec2 v_texCoord;

uniform mat4 model;
layout(std140, binding = 0) uniform matrix
{
	mat4 view;
	mat4 proj;
};

out vec2 f_texCoord;

void main()
{
	gl_Position = view * model * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}
#shader Geometry
#version 420 core

#define OFFSET_MAG 1.0

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 f_texCoord[];
out vec2 s_texCoord;

uniform float time;
layout(std140, binding = 0) uniform matrix
{
	mat4 view;
	mat4 proj;
};

void main()
{
	// calculate normal
	vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);

	vec3 norm = normalize(cross(b, a));
	// calculate offset
	vec3 offset = OFFSET_MAG * norm * time;

	for (int i = 0; i < 3; i++)
	{
		s_texCoord = f_texCoord[i];
		gl_Position = proj * (gl_in[i].gl_Position + vec4(offset, 0.0));
		EmitVertex();
	}
	EndPrimitive();
}

#shader Fragment
#version 420 core

in vec2 s_texCoord;
uniform sampler2D Texture1;

out vec4 color;

void main()
{
	color = texture(Texture1, s_texCoord);
}
