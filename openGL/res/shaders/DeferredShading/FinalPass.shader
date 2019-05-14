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
out vec4 color;
void main()
{
	color = texture(Texture, f_TexCoord);
}