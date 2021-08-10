#shader Vertex
#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;
layout(location = 3) in vec4 v_tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 v_lightPos;
uniform vec3 v_viewPos;

out VS_OUT{
	vec3 fragPos;
	vec2 texCoord;
	vec3 TanLightPos;
	vec3 TanViewPos;
	vec3 TanFragPos;
} vs;

void main()
{
	vec3 T = normalize(vec3(model * vec4(v_tangent.xyz, 0.0)));
	vec3 N = normalize(vec3(model * vec4(v_normal, 0.0)));
	vec3 B = v_tangent.w * cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));

	vs.fragPos = vec3(model * vec4(v_position, 1.0));
	vs.texCoord = v_texCoord;
	vs.TanLightPos = TBN * v_lightPos;
	vs.TanViewPos = TBN * v_viewPos;
	vs.TanFragPos = TBN * vs.fragPos;

	gl_Position = proj * view * model * vec4(v_position, 1.0);
}

#shader Fragment
#version 330 core

uniform sampler2D Texture1;
uniform sampler2D NormMap;
uniform sampler2D DispMap;

uniform int renderMode;

// Normalised
in VS_OUT{
	vec3 fragPos;
	vec2 texCoord;
	vec3 TanLightPos;
	vec3 TanViewPos;
	vec3 TanFragPos;
} vs;

out vec4 color;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
	vec3 viewDir = normalize(vs.TanViewPos - vs.TanFragPos);
	vec2 texCoord;
	if (renderMode == 2) // Parallax Mapping
	{
		texCoord = ParallaxMapping(vs.texCoord, viewDir);
		if (texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
			discard;
	}
	else
		texCoord = vs.texCoord;
	vec3 normal;
	if (renderMode == 1 || renderMode == 2)
	{
		// Normals need to be normalized from map
		normal = normalize(vec3(texture(NormMap, texCoord)));
		normal = normalize(normal * 2.0 - 1.0);
	}
	else
		normal = vec3(0.0, 0.0, 1.0);
	// Diffuse;
	vec3 lightDir = normalize(vs.TanFragPos - vs.TanLightPos);
	float diff = max(dot(-lightDir, normal), 0.0);
	// Specular
	float spec;

	vec3 halfDir = normalize(-lightDir + viewDir);
	spec = pow(max(dot(halfDir, normal), 0.0), 32);
	// Attenuation
	float distance = length(vs.TanLightPos - vs.TanFragPos);
	float constant = 1.0;
	float linear = 0.09;
	float quadratic = 0.032;
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	// combine results
	vec3 ambient = 0.3 * vec3(texture(Texture1, texCoord));
	vec3 diffuse = 0.9 * diff * vec3(texture(Texture1, texCoord));
	vec3 specular = vec3(0.3) * spec;

	color = vec4(attenuation * (ambient + diffuse + specular), 1.0);
}

#define height_scale 0.1
#define minLayers 8.0
#define maxLayers 32.0
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	// number of depth layers
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * height_scale;
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords = texCoords;
	float currentDepthMapValue = texture(DispMap, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(DispMap, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}
	// Interpolate using Parallax Occulsion Mapping

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	// get Depth before and after collision 
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(DispMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}