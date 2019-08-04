#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Base_Camera
{
public:
	Base_Camera();
	~Base_Camera();

	glm::vec3 getCameraPos() const;
	glm::vec3 getCameraDir() const;
	glm::vec3 getCameraRight() const;
	glm::vec3 getCameraForward() const;
	glm::mat4 getViewMatrix() const;
	float getFOV() const;

	void setSpeed(const float speed);
	void setPos(const glm::vec3 pos);

	bool InUse() const { return m_InUse; }

	virtual void resetPos() {  };
	virtual void key_callback(GLFWwindow* window, int key, int action) {  };
	virtual void mouse_callback(double xpos, double ypos) {  };
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {  };
	virtual bool handleWindowInput(GLFWwindow* window) { return false; };
protected:
	float m_FOV;
	float m_speed;
	
	bool m_firstMouse, m_InUse;

	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraDir;
	glm::vec3 m_cameraRight;
	glm::vec3 m_cameraForward;
};


#endif 
