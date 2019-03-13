#pragma once
#include <glm\glm.hpp>

class Shader
{
public:
	Shader(const char* shaderpath);
	~Shader();
	void Use() const;
	unsigned int getID() const;
private:
	unsigned int ID;
	unsigned CompileShader(std::string, GLenum);
public:
	void setInt(const std::string name, int value) const;
	void setBool(const std::string name, bool value) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
};