#shader Vertex
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_texCoord;

out vec2 f_texCoord;

void main()
{
	f_texCoord = v_texCoord;
	gl_Position = vec4(v_position, 0.0, 1.0);
}

#shader Fragment
#version 330 core
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform int renderMode;

in vec2 f_texCoord;

void main()
{
	color.a = 1.0;
	// Lighting
	if (renderMode == 0)
		color.rgb = vec3(0.4, 0.0, 0.4);

	else if (renderMode == 1)
		color.rgb = texture(gPosition, f_texCoord).rgb;
	else if (renderMode == 2)
		color.rgb = 0.5 * texture(gNormal, f_texCoord).rgb + vec3(0.5);
	else if (renderMode == 3)
		color.rgb = texture(gAlbedoSpec, f_texCoord).rgb;
	else if (renderMode == 4)
		color.rgb = texture(gAlbedoSpec, f_texCoord).aaa;
}
