#pragma once
#include <GL\glew.h>
class IndexBuffer
{
public:
	IndexBuffer(unsigned int *data, unsigned int count);
	~IndexBuffer();
	void Bind();
	void UnBind();
	unsigned int getID();
private:
	unsigned int ID;
};