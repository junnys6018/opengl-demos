#shader Vertex
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_texCoord;

out vec2 f_texCoord;

void main()
{
	gl_Position = vec4(v_position, 0.0, 1.0);
	f_texCoord = v_texCoord;
}

#shader Fragment
#version 330 core
out vec4 color;

in vec2 f_texCoord;

uniform sampler2D image;
uniform bool horizontal;
float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec3 result = texture(image, f_texCoord).rgb * weight[0]; // current fragments contibution
	
	if (horizontal)
	{
		for (int i = 1; i < 5; i++)
		{
			result += texture(image, f_texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, f_texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(image, f_texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(image, f_texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	color = vec4(result, 1.0);
}
