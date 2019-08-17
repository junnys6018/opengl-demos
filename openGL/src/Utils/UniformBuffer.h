#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <initializer_list>
#include <vector>
#include <cstdlib> // for copy()

#include "glm/glm.hpp"
#define GLEW_STATIC
#include "GL/glew.h"

enum std140_alignment
{
	MAT4 = 4 * sizeof(glm::vec4),
	MAT3 = 3 * sizeof(glm::vec4),

	INT = 4,
	BOOL = 4,
	FLOAT = 4,

	VEC2 = 8,
	VEC3 = 16,
	VEC4 = 16
};

class UniformBuffer
{
public:
	UniformBuffer(const std::initializer_list<std140_alignment>& layout, GLenum usage = GL_STATIC_DRAW);
	~UniformBuffer();
	void setData(unsigned int index, void* source, std140_alignment size);
	void Bind(int bindPoint);
	unsigned int ID;
private:
	std::vector<std140_alignment> m_layout;



};


#endif
