#shader Vertex
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_TexCoord;

out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(v_position, 0.0, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader Fragment
#version 330 core

out float color;
in vec2 f_TexCoord;
uniform sampler2D ssaoColor;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoColor, 0));
	float result = 0;
	for (int x = -2; x < 2; x++)
	{
		for (int y = -2; y < 2; y++)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoColor, f_TexCoord + offset).r;
		}
	}
	color = result / 16.0;
}