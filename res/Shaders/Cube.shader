#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 f_TexCoord;

void main()
{
	gl_Position = proj * view * model * vec4(v_position, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader Fragment
#version 330 core

in vec2 f_TexCoord;
uniform sampler2D Texture1;

out vec4 color;


void main()
{
	color = texture(Texture1, f_TexCoord);
}
