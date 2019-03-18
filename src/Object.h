#ifndef OBJECT_H
#define OBJECT_H

#include "IndexBuffer.h"
#include "VertexBuffer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

class Object
{
public:
	Object(const char* filepath);
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
};
#endif
