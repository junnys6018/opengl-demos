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

	unsigned VertexShader = CompileShader(Shaders[0], GL_VERTEX_SHADER);
	unsigned FragmentShader = CompileShader(Shaders[1], GL_FRAGMENT_SHADER);
	GLCall(ID = glCreateProgram());

	GLCall(glAttachShader(ID, VertexShader));
	GLCall(glAttachShader(ID, FragmentShader));
	GLCall(glLinkProgram(ID));
	GLCall(glValidateProgram(ID));
	// Link Error Handling
	int status;
	GLCall(glGetProgramiv(ID, GL_LINK_STATUS, &status));
	if (status == GL_FALSE)
	{
		int length;
		GLCall(glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length));
		char *message = new char[length];
		GLCall(glGetProgramInfoLog(ID, length, nullptr, message));
		std::cout << "Failed to link shaders!\n" << message << std::endl;
		delete[] message;
		GLCall(glDeleteProgram(ID));
	}

	GLCall(glDeleteShader(VertexShader));	// Delete Intermediates
	GLCall(glDeleteShader(FragmentShader));
	GLCall(glUseProgram(ID));
	}
Shader::~Shader()
{
	GLCall(glDeleteProgram(ID));
}
unsigned Shader::CompileShader(std::string source, GLenum type)
{
	GLCall(unsigned shader = glCreateShader(type));
	const char *c_source = source.c_str();
	GLCall(glShaderSource(shader, 1, &c_source, nullptr));
	GLCall(glCompileShader(shader));
	// Error handling
	int status;
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
	if (status == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
		char *message = new char[length];
		GLCall(glGetShaderInfoLog(shader, length, nullptr, message));
		std::cout << "FAILED TO COMPLIE " << (type == GL_VERTEX_SHADER ? "VERTEX " : "FRAGMENT ")
			<< "SHADER!\n" << message << '\n';
		delete[] message;
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



