#include "VertexBuffer.h"
#include "debug.h"

VertexBuffer::VertexBuffer(void *data, unsigned int size)
{
	GLCall(glGenBuffers(1, &ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &ID));
}
void VertexBuffer::Bind()
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
}
void VertexBuffer::unBind()
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
}
unsigned int VertexBuffer::getID()
{
	return ID;
}