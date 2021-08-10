#shader Vertex
#version 420 core

layout(location = 0) in vec3 v_position;

layout(std140, binding = 0) uniform Matrix
{
	mat4 view;
	mat4 proj;
};
layout(std140, binding = 1) uniform lights
{
	vec3[4] positions;
	vec3[4] colors;
};

out vec3 f_color;

mat4 translate(const mat4 m, const vec3 v);
mat4 scale(const mat4 m, const vec3 v);

void main()
{
	f_color = colors[gl_InstanceID];

	mat4 model = mat4(1.0);
	model = translate(model, positions[gl_InstanceID]);
	model = scale(model, vec3(0.5));
	gl_Position = proj * view * model * vec4(v_position, 1.0);
}

mat4 translate(const mat4 m,const vec3 v)
{
	mat4 result = mat4(m);
	result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
	return result;
}

mat4 scale(const mat4 m, const vec3 v)
{
	mat4 result = mat4(0.0);
	result[0] = m[0] * v.x;
	result[1] = m[1] * v.y;
	result[2] = m[2] * v.z;
	result[3] = m[3];
	return result;
}


#shader Fragment
#version 420 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec3 f_color;

void main()
{
	FragColor = vec4(f_color, 1.0);
	BrightColor = FragColor;
}

