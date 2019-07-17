#shader Vertex
#version 420 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_TexCoord;

out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(v_position, 0.0, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader Fragment
#version 420 core

out float FragColor;

in vec2 f_TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 proj;
uniform vec2 screenSize;
uniform float power;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(screenSize.x / 4.0, screenSize.y / 4.0);

void main()
{
	vec3 fragPos = texture(gPosition, f_TexCoord).xyz;
	vec3 normal = texture(gNormal, f_TexCoord).xyz;
	vec3 randomVec = texture(texNoise, f_TexCoord * noiseScale).xyz;

	// Generate TBN via the Gramm-Schmidt process
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	// TBN will be parrallel to normal vector with random rotation

	const int kernelSize = 64; const float radius = 0.5; const float bias = 0.025;
	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; ++i)
	{
		vec3 fsample = TBN * samples[i];
		fsample = fragPos + fsample * radius;
		// transform sample to clip space
		vec4 offset = vec4(fsample, 1.0);
		offset = proj * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;
		
		float sampleDepth = texture(gPosition, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= fsample.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - occlusion / float(kernelSize);
	FragColor = pow(occlusion, power);
}