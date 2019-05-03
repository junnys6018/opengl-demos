#pragma once
#include <GL\glew.h>
class IndexBuffer
{
public:
	IndexBuffer();
	IndexBuffer(unsigned int *data, unsigned int count);
	IndexBuffer(IndexBuffer&& other);
	IndexBuffer& operator=(IndexBuffer&& other);
	~IndexBuffer();
	void Bind();
	void UnBind();
	unsigned int getID();
	unsigned int getCount();
private:
	unsigned int ID;
	unsigned int count; // number of indices
};