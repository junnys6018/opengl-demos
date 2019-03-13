#pragma once
#include <GL\glew.h>
#include <string>

class Texture
{
public:
	Texture(const std::string filepath, GLenum format = GL_RGB);
	~Texture();
	void Bind(unsigned int slot = 0) const;
	void unBind() const;
	unsigned int getID();
private:
	unsigned int ID;
};