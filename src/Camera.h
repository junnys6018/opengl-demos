#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW\glfw3.h>

class Camera
{
public:
	Camera();
	glm::vec3 getCameraPos() const;
	glm::vec3 getCameraDir() const;
	glm::vec3 getCameraRight() const;
	glm::vec3 getCameraForward() const;

	glm::mat4 getViewMatrix() const;

	void key_callback(GLFWwindow *window, int key, int action);
	void mouse_callback(double xpos, double ypos);
	// moves position of camera based on key and cam dir
	void move(int dir);

private:
	float m_yaw, m_pitch;
	float m_lastX, m_lastY;
	float m_sensitivity, m_speed;
	// m_hascursor is false when GLFW_CURSOR_DISABLED is true
	bool m_firstMouse, m_hasCursor;

	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraDir;
	glm::vec3 m_cameraRight;
	glm::vec3 m_cameraForward;
};


#endif 
