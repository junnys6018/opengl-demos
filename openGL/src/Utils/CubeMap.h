#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "VertexArray.h"
#include "Shader.h"

#include "stb_image.h"

#include <vector>
#include <memory>

class CubeMap
{
public:
	CubeMap(std::vector<std::string> filepath);
	~CubeMap();
public:
	void Draw(const glm::mat4& VP);
private:
	unsigned int ID;
	static bool Initialised;
	static std::unique_ptr<VertexBuffer> skyBoxVB;
	static std::unique_ptr<VertexArray> skyBoxVA;
	static std::unique_ptr<Shader> s_cubeMap;
private:
	void loadCubeMap(std::vector<std::string> faces);
};
#endif
