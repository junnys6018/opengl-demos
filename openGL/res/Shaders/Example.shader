#shader Vertex
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec2 v_TexCoord;


out vec3 f_color;
out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(v_position, 0.0, 1.0);
	f_color = v_color;
	f_TexCoord = v_TexCoord;
}

#shader Fragment
#version 330 core

in vec2 f_TexCoord;
in vec3 f_color;
uniform sampler2D Texture1;
uniform bool isColored;
out vec4 color;


void main()
{
	if (isColored)
		color = texture(Texture1, f_TexCoord) * vec4(f_color, 1.0);
	else
		color = texture(Texture1, f_TexCoord);
}

