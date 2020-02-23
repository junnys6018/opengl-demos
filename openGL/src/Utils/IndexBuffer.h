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

	inline unsigned int getID() { return ID; }
	inline unsigned int getCount() { return count; }
private:
	unsigned int ID;
	unsigned int count; // number of indices
};