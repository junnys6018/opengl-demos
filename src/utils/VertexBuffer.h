#pragma once
#include <glad/glad.h>

class VertexBuffer
{
public:
	VertexBuffer(void *data, unsigned int size);
	VertexBuffer(unsigned int size);
	~VertexBuffer();
public:
	void Bind() const;
	void unBind() const;
	unsigned int getID() const;
private:
	unsigned int ID;
};