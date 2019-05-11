#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_TexCoord;

uniform mat4 MVP;
uniform mat4 model;

out vec2 f_TexCoord;
out vec3 f_position;
out vec3 f_normal;

void main()
{
	gl_Position = MVP * vec4(v_position, 1.0);
	f_TexCoord = v_TexCoord;
	f_position = vec3(model * vec4(v_position, 1.0));
	f_normal = normalize(mat3(transpose(inverse(model))) * v_normal);
}

#shader Fragment
#version 330 core

in vec2 f_TexCoord;
in vec3 f_position;
in vec3 f_normal;

uniform sampler2D Texture1;
uniform samplerCube skybox;
uniform vec3 camPos;

out vec4 color;

void main()
{
	vec3 viewDir = normalize(f_position - camPos);
	vec3 reflect = reflect(viewDir, f_normal);
	color = vec4(texture(skybox, reflect).rgb, 1.0) * texture(Texture1, f_TexCoord);
}
