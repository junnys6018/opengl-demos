#shader Vertex
#version 330 core
layout(location = 0) in vec3 v_position;

out vec3 f_position;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	f_position = v_position;
	gl_Position = proj * view * vec4(v_position, 1.0);
}

#shader Fragment
#version 330 core
out vec4 FragColor;
in vec3 f_position;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(f_position)); // make sure to normalize localPos
	vec3 color = texture(equirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}
