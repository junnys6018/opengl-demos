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

uniform samplerCube environmentMap;
const float PI = 3.14159265359;
void main()
{
	vec3 normal = normalize(f_position);
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}
