#pragma once
#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>

/*	Shader class loads, comlies and links shader from a single .shader file. Vertex and fragment shaders are
 *	seperated by #shader Vertex and #shader Fragment in .shader file.
 */
class Shader
{
public:
	Shader(const std::string& shaderpath);
	~Shader();
public:
	void Use() const;
	unsigned int getID() const;
	// Functions sets Uniforms in the shader code
	void setInt(const std::string &name, const int &value) const;
	void setBool(const std::string &name, const bool &value) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
	void setVec3(const std::string &name, const float &x, const float & y, const float & z) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec2(const std::string &name, const float &x, const float &y) const;
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setFloat(const std::string& name, const float& value) const;
private:
	unsigned CompileShader(std::string, GLenum);
	bool validateShader();
	GLint getUniformLocation(const std::string &name) const;
private:
	unsigned int ID;
	bool failedToLoad, isComputeShader;
	std::string m_shaderpath;
	mutable std::map<std::string, GLint> m_uniformLocations;
};