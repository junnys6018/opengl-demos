#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
uniform mat4 VP;

out vec3 f_TexCoord;

void main()
{
	f_TexCoord = v_position;
	vec4 pos = VP * vec4(v_position, 1.0);
	gl_Position = pos.xyww;
}

#shader Fragment
#version 330 core

in vec3 f_TexCoord;
uniform samplerCube skybox;

out vec4 color;

void main()
{
	color = texture(skybox, f_TexCoord);
}
