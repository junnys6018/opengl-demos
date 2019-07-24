#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	glm::vec3 getCameraPos() const;
	glm::vec3 getCameraDir() const;
	glm::vec3 getCameraRight() const;
	glm::vec3 getCameraForward() const;
	glm::mat4 getViewMatrix() const;
	float getDeltaT() const;
	float getFOV() const;

	void setSpeed(const float speed);
	void setFOV(const float FOV);

	bool InUse() const { return m_InUse; }


	void key_callback(GLFWwindow *window, int key, int action);
	void mouse_callback(double xpos, double ypos);
	// moves position of camera based on key and cam dir
	bool move(GLFWwindow* window);
	void resetPos();

private:
	float m_FOV;
	float m_yaw, m_pitch;
	float m_lastX, m_lastY;
	float m_sensitivity, m_speed;
	
	bool m_firstMouse, m_InUse;
	float prevtime, currtime, delta;
	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraDir;
	glm::vec3 m_cameraRight;
	glm::vec3 m_cameraForward;
};


#endif 
