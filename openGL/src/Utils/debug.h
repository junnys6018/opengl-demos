#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#define GLEW_STATIC
#include <GL\glew.h>

#include <string>
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
		std::string error;
		switch (err)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default:							   error = "ENUM NOT IMPLEMENTED, CODE: " + std::to_string(err); break;
		}
		std::cout << "[OPENGL ERROR] (" << error << ") LINE: " << line << " FILE: "
			<< file << std::endl;
	}
}
#else
#define GLCall(x)
#endif

#endif
