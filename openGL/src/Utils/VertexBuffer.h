#pragma once
#define GLEW_STATIC
#include <GL\glew.h>

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