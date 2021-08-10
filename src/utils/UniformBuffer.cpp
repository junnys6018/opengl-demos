#include "UniformBuffer.h"
#include "debug.h"
UniformBuffer::UniformBuffer(const std::initializer_list<std140_alignment>& layout, GLenum useage)
{
	int bufSize = 0;
	for (auto e : layout)
	{
		m_layout.push_back(e);
		bufSize += (int)e;
	}

	GLCall(glGenBuffers(1, &ID));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, ID));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, bufSize, nullptr, useage));
}

UniformBuffer::~UniformBuffer()
{
	GLCall(glDeleteBuffers(1, &ID));
}

void UniformBuffer::setData(unsigned int index, void* source, std140_alignment size)
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, ID));
	int offset = 0;
	for (int i = 0; i < index; ++i)
		offset += (int)m_layout[i];
	glBufferSubData(GL_UNIFORM_BUFFER, offset, (int)size, source);

}

void UniformBuffer::Bind(int bindPoint)
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, ID));
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, ID));
}
