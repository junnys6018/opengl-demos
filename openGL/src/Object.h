#ifndef OBJECT_H
#define OBJECT_H

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "glm/glm.hpp"
enum Object_Init_Flags
{
	OBJECT_INIT_FLAGS_NONE = 0,
	OBJECT_INIT_FLAGS_GEN_TANGENT = 1
};
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
struct Material
{
	std::string m_mtlName;
	std::unique_ptr<Texture> m_texture;
	Material(std::string mtlName)
		:m_mtlName(mtlName)
	{

	}
	void genTexture(std::string texPath)
	{
		m_texture = std::make_unique<Texture>(texPath);
	}
};
struct Material_ptr // assigns a block of indicies to a material
{
	int m_count;
	unsigned int m_offset;
	std::string m_mtlName;
	Material* m_material;
	Material_ptr(int count, unsigned int offset, std::string mtlName = "")
		:m_count(count), m_offset(offset), m_mtlName(mtlName), m_material(nullptr)
	{

	}
};
class Object
{
public:
	Object() = default;
	Object(const std::string& filepath, Object_Init_Flags flags = OBJECT_INIT_FLAGS_NONE);
	std::unique_ptr<VertexBuffer> vertexBuffer;
	std::unique_ptr<IndexBuffer> indexBuffer;
	std::unique_ptr<VertexArray> vertexArray;
	void Draw(const Shader& shader, bool isTextured = true);

private:
	std::vector<Material_ptr> mat_ptrs;
	std::vector<Material> materials;

	const std::vector<std::string> parse_obj(const std::string filepath);
	void parse_mtl(const std::string filepath);
	void genTangents(std::vector<Vertex> vBuf, std::vector<unsigned int> iBuf);
	std::string calc_root_dir(std::string filepath);
};
#endif