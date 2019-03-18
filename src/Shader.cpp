#include <GL/glew.h>

#include <string>
#include <fstream>
#include <iostream>

#include "debug.h"
#include "Shader.h"


Shader::Shader(const char* shaderpath)
{
	// PARSE SHADER
	std::string Shaders[2];
	enum class Mode
	{
		VOID = -1, VERTEX_SHADER = 0, FRAGMENT_SHADER = 1
	};
	// Seperates .shader file into 2 strings, one holding the vertex shader, the other holding the fragment shader
	Mode mode = Mode::VOID;
	std::ifstream file(shaderpath);
	std::string line;
	while (std::getline(file, line))
	{
		if (line == "#shader Vertex")
			mode = Mode::VERTEX_SHADER;
		else if (line == "#shader Fragment")
			mode = Mode::FRAGMENT_SHADER;
		else if (mode != Mode::VOID)
			Shaders[(int)mode] += line + '\n';
	}
	// complies shader and returns ID to shader object.
	unsigned VertexShader = CompileShader(Shaders[0], GL_VERTEX_SHADER);
	unsigned FragmentShader = CompileShader(Shaders[1], GL_FRAGMENT_SHADER);
	// create program object
	GLCall(ID = glCreateProgram());
	// attach and link vertex and fragment shader
	GLCall(glAttachShader(ID, VertexShader));
	GLCall(glAttachShader(ID, FragmentShader));
	GLCall(glLinkProgram(ID));
	GLCall(glValidateProgram(ID));
	// Error Handling for linking stage
	int status;
	GLCall(glGetProgramiv(ID, GL_LINK_STATUS, &status));
	if (status == GL_FALSE)
	{
		int length;
		GLCall(glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length));
		if (length != 0)
		{
			char *message = new char[length];
			GLCall(glGetProgramInfoLog(ID, length, nullptr, message));
			std::cout << "Failed to link shaders!\n" << message << std::endl;
			delete[] message;
		}
		GLCall(glDeleteProgram(ID));
	}
	else
	{
		// Deletes intermediates
		GLCall(glDeleteShader(VertexShader));
		GLCall(glDeleteShader(FragmentShader));
		GLCall(glUseProgram(ID));
	}
}
Shader::~Shader()
{
	GLCall(glDeleteProgram(ID));
}
unsigned Shader::CompileShader(std::string source, GLenum type)
{
	// create shader object
	GLCall(unsigned shader = glCreateShader(type));
	// load source code into shader object
	const char *c_source = source.c_str();
	GLCall(glShaderSource(shader, 1, &c_source, nullptr));
	// complie shader program
	GLCall(glCompileShader(shader));
	// Error handling for compling shaders
	int status;
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
	if (status == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
		if (length != 0)
		{
			char *message = new char[length];
			GLCall(glGetShaderInfoLog(shader, length, nullptr, message));
			std::cout << "FAILED TO COMPLIE " << (type == GL_VERTEX_SHADER ? "VERTEX " : "FRAGMENT ")
				<< "SHADER!\n" << message << '\n';
			delete[] message;
		}
	}
	return shader;
}

void Shader::Use() const
{
	GLCall(glUseProgram(ID));
}
unsigned int Shader::getID() const
{
	return ID;
}
// setter functions for shader uniforms
void Shader::setInt(const std::string name, int value) const
{
	GLCall(glUseProgram(ID));
	GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), value));
}
void Shader::setBool(const std::string name, bool value) const
{
	GLCall(glUseProgram(ID));
	GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), value));
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	GLCall(glUseProgram(ID));
	GLCall(glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
}



