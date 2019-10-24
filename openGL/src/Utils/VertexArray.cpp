#include "debug.h"
#include "VertexArray.h"
// Vertex array object wrapper class
VertexArray::VertexArray(const VertexBuffer &vBuf, GLenum type, std::initializer_list<unsigned int> AttribLayout)
	:ID(0), typeSize(0), numAttribs(0), hasIndexBuffer(false)
{
	typeSize = getSize(type);
	numAttribs = AttribLayout.size();

	if (typeSize)
	{
		GLCall(glGenVertexArrays(1, &ID));
		GLCall(glBindVertexArray(ID));

		vBuf.Bind();

		unsigned int stride = 0;
		auto it = AttribLayout.begin();
		// calculates stride of each vertex
		for (/*NULL*/; it != AttribLayout.end(); ++it)
			stride += *it * typeSize;
		unsigned int offSet = 0;
		it = AttribLayout.begin();
		for (int i = 0; i != AttribLayout.size(); ++i, ++it)
		{
			GLCall(glVertexAttribPointer(i, *it, type, GL_FALSE, stride, (void*)(offSet * typeSize)));
			GLCall(glEnableVertexAttribArray(i));
			offSet += *it;
		}
		GLCall(glBindVertexArray(0));
	}

}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &ID));
}
// Returns size of type in bytes
unsigned int VertexArray::getSize(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
		return sizeof(float);
	case GL_UNSIGNED_INT:
		return sizeof(unsigned int);
	default:
		std::cout << "[VertexArray.cpp] Invalid AttribLayout type (" << type
			<< ") , please implement in getSize() function";
		return 0;
	}
}
void VertexArray::Bind()
{
	GLCall(glBindVertexArray(ID));
}

void VertexArray::unBind()
{
	GLCall(glBindVertexArray(0));
}

unsigned int VertexArray::getID()
{
	return ID;
}

void VertexArray::addIndexBuffer(IndexBuffer &iBuf)
{
	GLCall(glBindVertexArray(ID));
	iBuf.Bind();
	GLCall(glBindVertexArray(0));
	hasIndexBuffer = true;
}
