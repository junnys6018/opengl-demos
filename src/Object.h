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
	Object(const std::string& filepath);
	std::unique_ptr<VertexBuffer> vertexBuffer;
	std::unique_ptr<IndexBuffer> indexBuffer;
	std::unique_ptr<VertexArray> vertexArray;
	void Draw(const Shader& shader, bool isTextured = true);

private:
	std::vector<Material_ptr> mat_ptrs;
	std::vector<Material> materials;

	const std::vector<std::string> parse_obj(const std::string filepath);
	void parse_mtl(const std::string filepath);
	std::string calc_root_dir(std::string filepath);
};
#endif
