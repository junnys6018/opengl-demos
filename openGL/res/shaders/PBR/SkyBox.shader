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
uniform float mip;

out vec4 color;

void main()
{
	if (mip < 0.1)
	{
		color = vec4(texture(skybox, f_TexCoord).rgb, 1.0);
	}
	else
	{
		color = vec4(textureLod(skybox, f_TexCoord, mip).rgb, 1.0);
	}
}
