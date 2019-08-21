#pragma once
#include <GL/glew.h>

#include <iostream>
#include <string>
#include <stb_image.h>
enum Texture_Init_Flags
{
	TEXTURE_INIT_FLAGS_NONE = 0,
	TEXTURE_INIT_FLAGS_GAMMA_CORRECT = 1,
	TEXTURE_INIT_FLAGS_NOFLIP = 1 << 1,
	TEXTURE_INIT_FLAGS_GEN_MIPMAP = 1 << 2
};
Texture_Init_Flags operator|(Texture_Init_Flags lhs, Texture_Init_Flags rhs);
// Texture wrapper class
class Texture
{
public:
	Texture(const std::string& filepath, int wrap = GL_REPEAT, Texture_Init_Flags flags = TEXTURE_INIT_FLAGS_NONE);
	Texture(float color[4]);
	~Texture();
	void Bind(unsigned int slot = 0) const;
	void unBind() const;
	unsigned int getID();
private:
	unsigned int ID;
};