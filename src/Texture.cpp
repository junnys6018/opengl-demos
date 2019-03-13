#include "Texture.h"
#include "debug.h"

#include <iostream>
#include <stb_image.h>

Texture::Texture(const std::string filepath, GLenum format)
{
	stbi_set_flip_vertically_on_load(true);
	GLCall(glGenTextures(1, &ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));
	// set the ID wrapping/filtering options (on the currently bound ID object)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// load and generate the ID
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}
Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &ID));
}
void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // activate the texture unit first before binding texture
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));
}
void Texture::unBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
unsigned int Texture::getID()
{
	return ID;
}