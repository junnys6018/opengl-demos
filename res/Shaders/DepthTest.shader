#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texCoord;

uniform mat4 MVP;

out vec2 f_texCoord;

void main()
{
	gl_Position = MVP * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}

#shader Fragment
#version 330 core

uniform sampler2D tex;
uniform bool visDepBuf;
uniform bool visLinDepBuf;
in vec2 f_texCoord;
out vec4 FragColor;

#define NEAR 0.1
#define FAR 30.0
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
	if (visDepBuf)
		FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
	else if (visLinDepBuf)
		FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / FAR), 1.0);
	else
		FragColor = texture(tex, f_texCoord);
}
