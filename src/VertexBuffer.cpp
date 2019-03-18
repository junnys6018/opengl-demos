#include "VertexBuffer.h"
#include "debug.h"
VertexBuffer::VertexBuffer()
	:ID(0) {	}
VertexBuffer::VertexBuffer(void *data, unsigned int size)
{
	GLCall(glGenBuffers(1, &ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(VertexBuffer&& other)
	: ID(other.ID)
{
	other.ID = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
{
	unsigned id = ID;
	ID = other.ID;
	other.ID = id;
	return *this;
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &ID));
}
void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
}
void VertexBuffer::unBind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
}
unsigned int VertexBuffer::getID() const
{
	return ID;
}