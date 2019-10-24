#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <initializer_list>
#include <iostream>
// Vertex array object wrapper class
class VertexArray
{
public:
	VertexArray(const VertexBuffer &vBuf, GLenum type, std::initializer_list<unsigned int> AttribLayout);
	~VertexArray();
public:
	void Bind();
	void unBind();
	unsigned int getID();
	void addIndexBuffer(IndexBuffer &iBuf);
private:
	unsigned int ID;
	unsigned int typeSize;
	unsigned int numAttribs;
	bool hasIndexBuffer;
private:
	unsigned int getSize(GLenum type);
};


#endif
