#pragma once
#include <GL\glew.h>

/*	VertexBuffer class manages a vertex buffer object. Constructor generates a new buffer, 
 *	binds it to GL_ARRAY_BUFFER and buffers the provided data to vram
 */
class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(void *data, unsigned int size);
	VertexBuffer(VertexBuffer&& other);
	VertexBuffer& operator=(VertexBuffer&& other);
	~VertexBuffer();
	void Bind() const;
	void unBind() const;
	unsigned int getID() const;
private:
	unsigned int ID;
};