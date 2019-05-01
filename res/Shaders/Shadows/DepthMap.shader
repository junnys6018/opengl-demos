#shader Vertex
#version 420 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_TexCoord;

out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(v_position, 0.0, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader Fragment
#version 420 core

in vec2 f_TexCoord;
uniform sampler2D depthMap;

out vec4 color;


void main()
{
	float depth = texture(depthMap, f_TexCoord).r;
	color = vec4(vec3(depth), 1.0);
}
