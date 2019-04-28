#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"

#include "STB_IMAGE/stb_image.h"

#include <vector>
#include <memory>

class CubeMap
{
public:
	CubeMap(std::vector<std::string> filepath);
	~CubeMap();
	void Draw(const glm::mat4& VP);
private:
	unsigned int texture_handle;
	std::unique_ptr<VertexBuffer> skyBoxVB;
	std::unique_ptr<VertexArray> skyBoxVA;
	std::unique_ptr<Shader> s_cubeMap;

	void loadCubeMap(std::vector<std::string> faces);
};
#endif
