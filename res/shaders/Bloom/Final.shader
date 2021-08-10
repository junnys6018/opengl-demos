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

in vec2 f_texCoord;
uniform sampler2D scene;
uniform sampler2D blur;
uniform float exposure;
uniform int renderMode;
out vec4 color;


void main()
{
	vec3 result = vec3(1.0);
	if (renderMode == 0)
		result = texture(scene, f_texCoord).rgb;
	else if (renderMode == 1)
		result = texture(blur, f_texCoord).rgb;
	else if (renderMode == 2)
		result = texture(scene, f_texCoord).rgb + texture(blur, f_texCoord).rgb;
	// Tone Mapping
	result = vec3(1.0) - exp(-result * exposure);

	color = vec4(result, 1.0);
}
