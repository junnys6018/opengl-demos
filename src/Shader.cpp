#include "debug.h"
#include "Shader.h"


Shader::Shader(const std::string& shaderpath)
	:failedToLoad(false)
{
	// PARSE SHADER
	std::string Shaders[3];
	enum class Mode
	{
		VOID = -1, VERTEX_SHADER = 0, FRAGMENT_SHADER = 1, GEOMETRY_SHADER = 2
	};
	// Seperates .shader file into 23strings, one holding the vertex shader, the other holding the fragment shader, the last holding geom shader
	Mode mode = Mode::VOID;
	std::ifstream file(shaderpath);
	std::string line;
	while (std::getline(file, line))
	{
		if (line == "#shader Vertex")
			mode = Mode::VERTEX_SHADER;
		else if (line == "#shader Fragment")
			mode = Mode::FRAGMENT_SHADER;
		else if (line == "#shader Geometry")
			mode = Mode::GEOMETRY_SHADER;
		else if (mode != Mode::VOID)
			Shaders[(int)mode] += line + '\n';
	}
	// complies shader and returns ID to shader object.
	unsigned int VertexShader = CompileShader(Shaders[0], GL_VERTEX_SHADER);
	unsigned int FragmentShader = CompileShader(Shaders[1], GL_FRAGMENT_SHADER);
	unsigned int GeometryShader = 0;
	if (Shaders[2] != "")
		GeometryShader = CompileShader(Shaders[2], GL_GEOMETRY_SHADER);
	// create program object
	GLCall(ID = glCreateProgram());
	// attach and link vertex and fragment shader
	GLCall(glAttachShader(ID, VertexShader));
	GLCall(glAttachShader(ID, FragmentShader));
	if (Shaders[2] != "")
	{
		GLCall(glAttachShader(ID, GeometryShader));
	}
	GLCall(glLinkProgram(ID));
	GLCall(glValidateProgram(ID));
	// Error Handling for linking stage
	int status;
	GLCall(glGetProgramiv(ID, GL_LINK_STATUS, &status));
	if (status == GL_FALSE)
	{
		std::cout << "Failed to link shaders!\n";
		int length;
		GLCall(glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length));
		if (length != 0)
		{
			char *message = new char[length];
			GLCall(glGetProgramInfoLog(ID, length, nullptr, message));
			std::cout << message << std::endl;
			delete[] message;
		}
		GLCall(glDeleteProgram(ID));
		failedToLoad = true;
	}
	else
	{
		// Deletes intermediates
		GLCall(glDeleteShader(VertexShader));
		GLCall(glDeleteShader(FragmentShader));
		if (Shaders[2] != "")
		{
			GLCall(glDeleteShader(GeometryShader));
		}
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
	GLCall(unsigned int shader = glCreateShader(type));
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
			std::cout << "FAILED TO COMPLIE " << (type == GL_VERTEX_SHADER ? "VERTEX " : 
				(type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "GEOMETRY"))
				<< "SHADER!\n" << message << '\n';
			delete[] message;
		}
		failedToLoad = true;
	}
	return shader;
}

void Shader::Use() const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
	}
}
unsigned int Shader::getID() const
{
	return ID;
}
// setter functions for shader uniforms
void Shader::setInt(const std::string &name, const int &value) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform1i(location, value));
	}
}
void Shader::setBool(const std::string &name, const bool &value) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform1i(location, value));
	}
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]));
	}
}
void Shader::setVec3(const std::string &name, const float &x, const float &y, const float &z) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform3f(location, x, y, z));
	}
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform3f(location, value.x, value.y, value.z));
	}
}
void Shader::setVec2(const std::string& name, const float& x, const float& y) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform2f(location, x, y));
	}
}
void Shader::setVec2(const std::string & name, const glm::vec2 & value) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform2f(location, value.x, value.y));
	}
}
void Shader::setFloat(const std::string &name, const float &value) const
{
	if (!failedToLoad)
	{
		GLCall(glUseProgram(ID));
		GLCall(int location = glGetUniformLocation(ID, name.c_str()));
		if (location == -1)
			std::cout << name << " is not a valid uniform\n";
		else GLCall(glUniform1f(location, value));
	}
}

