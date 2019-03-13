#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#include <GL\glew.h>
#include <iostream>

#define GLCall(x) GLClearError();\
	x;\
	GLLogCall(__LINE__, __FILE__)

inline void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
inline void GLLogCall(unsigned line, const char* file)
{
	while (GLenum err = glGetError())
	{
		std::cout << "[OPEN GL ERROR] (" << err << ") LINE: " << line << " FILE: "
			<< file << std::endl;
	}
}
#else
#define GLCall(x) x
#endif

#endif
