#pragma once
#include <GL/glew.h>

#include <iostream>
#include <string>
#include <stb_image.h>
// Texture wrapper class
class Texture
{
public:
	Texture(const std::string& filepath, int wrap = GL_REPEAT, bool is_sRGB_space = false);
	~Texture();
	void Bind(unsigned int slot = 0) const;
	void unBind() const;
	unsigned int getID();
private:
	unsigned int ID;
};