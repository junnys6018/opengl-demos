#pragma once
#include <GL\glew.h>
class VertexBuffer
{
public:
	VertexBuffer(void *data, unsigned int size);
	~VertexBuffer();
	void Bind();
	void unBind();
	unsigned int getID();
private:
	unsigned int ID;
};