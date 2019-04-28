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

in vec2 f_TexCoord;
uniform sampler2D Texture;
uniform int flags;
uniform float offset;
out vec4 color;

vec4 convMat(float kernel[9], vec2 offsets[9]);
void main()
{
	// no post-processing
	if (flags == 0)
		color = texture(Texture, f_TexCoord);
	// inversion
	else if (flags == 1)
		color = vec4(vec3(1.0 - texture(Texture, f_TexCoord)), 1.0);
	// gray scale
	else if (flags == 2)
	{
		color = texture(Texture, f_TexCoord);
		// use weighted average bc human eyes are more sensitive to green and less blue
		float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
		color = vec4(average, average, average, 1.0);
	}
	else
	{
		vec2 offsets[9] = vec2[](
			vec2(-offset, offset),  // top-left
			vec2(0.0f, offset),     // top-center
			vec2(offset, offset),   // top-right
			vec2(-offset, 0.0f),    // center-left
			vec2(0.0f, 0.0f),       // center-center
			vec2(offset, 0.0f),     // center-right
			vec2(-offset, -offset), // bottom-left
			vec2(0.0f, -offset),    // bottom-center
			vec2(offset, -offset)   // bottom-right  
			);
		// sharpen
		if (flags == 3)
		{
			float kernel[9] = float[](
				-1, -1, -1,
				-1, 9, -1,
				-1, -1, -1
				);
			color = convMat(kernel, offsets);
		}
		// blur
		else if (flags == 4)
		{
			float kernel[9] = float[](
				1.0 / 16, 2.0 / 16, 1.0 / 16,
				2.0 / 16, 4.0 / 16, 2.0 / 16,
				1.0 / 16, 2.0 / 16, 1.0 / 16
				);
			color = convMat(kernel, offsets);
		}
		// edge detection
		else if (flags == 5)
		{
			float kernel[9] = float[](
				1, 1, 1,
				1, -8, 1,
				1, 1, 1
				);
			color = convMat(kernel, offsets);
		}
		
	}
}

vec4 convMat(float kernel[9], vec2 offsets[9])
{
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(Texture, f_TexCoord.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];

	return vec4(col, 1.0);
}
