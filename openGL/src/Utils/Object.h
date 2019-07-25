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
#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
enum Object_Init_Flags
{
	OBJECT_INIT_FLAGS_NONE =				0,
	OBJECT_INIT_FLAGS_GEN_TANGENT =			1 << 0,
	OBJECT_INIT_FLAGS_GAMMA_CORRECT =		1 << 1,
	OBJECT_INIT_FLAGS_NOFLIP =				1 << 2,
	OBJECT_INIT_FLAGS_GEN_MIPMAP =			1 << 3,

	OBJECT_INIT_FLAGS_GEN_TEXTURE =         1 << 4,
	OBJECT_INIT_FLAGS_GEN_NORMAL =          1 << 5,
	OBJECT_INIT_FLAGS_GEN_SPECULAR =        1 << 6,
};
Object_Init_Flags operator|(Object_Init_Flags lhs, Object_Init_Flags rhs);
enum Draw_Flags
{
	DRAW_FLAGS_NONE = 0,
	DRAW_FLAGS_DIFFUSE = 1,
	DRAW_FLAGS_SPECULAR = 1 << 1,
	DRAW_FLAGS_NORMAL = 1 << 2,

	DRAW_FLAGS_TRIANGLE_STRIP = 1 << 3,
};
Draw_Flags operator|(Draw_Flags lhs, Draw_Flags rhs);

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
// TODO : REDO MATERIALS
struct Material
{
	std::string m_mtlName;
	bool hasTexture, hasNormMap, hasSpecMap;
	std::unique_ptr<Texture> m_texture;
	std::unique_ptr<Texture> m_NormMap;
	std::unique_ptr<Texture> m_SpecMap;
	Material(std::string mtlName)
		:m_mtlName(mtlName), hasTexture(false), hasNormMap(false), hasSpecMap(false)
	{

	}
	void genTexture(std::string texPath, GLenum wrap, Texture_Init_Flags flags)
	{
		m_texture = std::make_unique<Texture>(texPath, wrap, flags);
		hasTexture = true;
	}
	void genNormMap(std::string texPath)
	{
		m_NormMap = std::make_unique<Texture>(texPath);
		hasNormMap = true;
	}
	void genSpecMap(std::string texPath)
	{
		m_SpecMap = std::make_unique<Texture>(texPath);
		hasSpecMap = true;
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
	void Draw(const Shader& shader, Draw_Flags flags = DRAW_FLAGS_DIFFUSE);

private:
	std::vector<Material_ptr> mat_ptrs;
	std::vector<Material> materials;

	const std::vector<std::string> parse_obj(const std::string filepath, std::vector<Vertex>& vBuf, std::vector<unsigned int>& iBuf);
	void parse_mtl(const std::string filepath, Object_Init_Flags flags);
	void genTangents(const std::vector<Vertex>& vBuf,const std::vector<unsigned int>& iBuf);
	std::string calc_root_dir(std::string filepath);
};
#endif