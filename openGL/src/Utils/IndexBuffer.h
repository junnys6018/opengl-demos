#pragma once
#define GLEW_STATIC
#include <GL\glew.h>
class IndexBuffer
{
public:
	IndexBuffer(unsigned int *data, unsigned int count);
	~IndexBuffer();
public:
	void Bind();
	void UnBind();
	unsigned int getID();
	unsigned int getCount();
private:
	unsigned int ID;
	unsigned int count; // number of indices
};