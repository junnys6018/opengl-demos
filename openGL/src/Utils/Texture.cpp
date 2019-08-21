#include "Texture.h"
#include "debug.h"

#include <assert.h>

// Forward declaration
std::string rel_to_abs_filepath(const std::string& relative);

Texture_Init_Flags operator|(Texture_Init_Flags lhs, Texture_Init_Flags rhs)
{
	return (Texture_Init_Flags)((int)lhs | (int)rhs);
}
// Texture wrapper class
Texture::Texture(const std::string& filepath, int wrap, Texture_Init_Flags flags)
{
	if (flags & TEXTURE_INIT_FLAGS_NOFLIP)
		stbi_set_flip_vertically_on_load(false);
	else
		stbi_set_flip_vertically_on_load(true);
	GLCall(glGenTextures(1, &ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));
	// set the ID wrapping/filtering options (on the currently bound ID object)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_INIT_FLAGS_GEN_MIPMAP ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR)));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// load and generate the ID
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0); // for some reason relative file path is not accepted
	if (data)
	{
		GLenum internal_fmt, fmt;
		if (nrChannels == 1)
			fmt = GL_RED;
		else if (nrChannels == 3)
			fmt = GL_RGB;
		else if (nrChannels == 4)
			fmt = GL_RGBA;
		if (flags & TEXTURE_INIT_FLAGS_GAMMA_CORRECT)
		{
			if (nrChannels == 3)
				internal_fmt = GL_SRGB;
			else if (nrChannels == 4)
				internal_fmt = GL_SRGB_ALPHA;
		}
		else
		{
			internal_fmt = fmt;
		}
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data));
		if (flags & TEXTURE_INIT_FLAGS_GEN_MIPMAP)
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		const char* log = stbi_failure_reason();
		std::cout << "Failed to load texture at " << filepath << ' ' << (log ? log : "") << std::endl;
	}
	stbi_image_free(data);
}

// Generates solid color texture
Texture::Texture(float color[4])
{
	GLCall(glGenTextures(1, &ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));
	// set the ID wrapping/filtering options (on the currently bound ID object)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	unsigned char data[4];
	for (int i = 0; i < 4; i++)
	{
		assert(color[i] <= 1.0f && color[i] >= 0.0f);
		data[i] = 255 * color[i];
	}

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
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

std::string rel_to_abs_filepath(const std::string& relative)
{
	return std::string("C:/Users/Jun Lim/source/repos/openGL/openGL/") + relative;
}