#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
#include <GL\glew.h>
#include <initializer_list>
#include <iostream>

#include "debug.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


class VertexArray
{
public:
	VertexArray(const VertexBuffer &vBuf, GLenum type, std::initializer_list<unsigned int> AttribLayout);
	VertexArray(VertexArray&& other);
	VertexArray& operator=(VertexArray&& other);
	~VertexArray();
	void Bind();
	void unBind();
	unsigned int getID();
	void addIndexBuffer(IndexBuffer &iBuf);
private:
	unsigned int ID;
	unsigned int typeSize;
	unsigned int numAttribs;
	bool hasIndexBuffer;
	unsigned int getSize(GLenum type);
};


#endif
