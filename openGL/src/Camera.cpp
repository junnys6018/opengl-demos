#include "Camera.h"

#include <iostream>

Camera::Camera()
	:m_yaw(270.0f), m_pitch(0.0f), m_lastX(0.0f), m_lastY(0.0f), m_FOV(45.0f)
	, m_sensitivity(0.1f), m_speed(0.08f), m_firstMouse(true), m_InUse(false)
{
	resetPos();
}
glm::vec3 Camera::getCameraPos() const
{
	return m_cameraPos;
}
glm::vec3 Camera::getCameraDir() const
{
	return m_cameraDir;
}
glm::vec3 Camera::getCameraRight() const
{
	return m_cameraRight;
}
glm::vec3 Camera::getCameraForward() const
{
	return m_cameraForward;
}
glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f));
}
void Camera::setSpeed(const float speed)
{
	m_speed = speed;
}
void Camera::resetPos()
{
	m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);	// Position in worldspace
	m_cameraDir = glm::vec3(0.0f, 0.0f, -1.0f);	// direction the camera is facing relative to position in world space
	m_cameraRight = glm::normalize(glm::cross(m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_cameraForward = glm::vec3(m_cameraRight.z, 0.0f, -m_cameraRight.x);
}
void Camera::mouse_callback(double xpos, double ypos)
{
	if (m_InUse)
	{
		if (m_firstMouse) // this bool variable is initially set to true
		{
			m_lastX = xpos;
			m_lastY = ypos;
			m_firstMouse = false;
		}
		float xoffset = xpos - m_lastX;
		float yoffset = m_lastY - ypos; // reversed since y-coordinates range from bottom to top
		m_lastX = xpos;
		m_lastY = ypos;
		// scale down offset values
		xoffset *= m_sensitivity;
		yoffset *= m_sensitivity;
		// Update yaw and pitch
		m_yaw += xoffset;
		m_pitch += yoffset;
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;
		// Update camera vectors
		m_cameraDir.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
		m_cameraDir.y = sin(glm::radians(m_pitch));
		m_cameraDir.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
		m_cameraRight = glm::normalize(glm::cross(m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_cameraForward = glm::vec3(m_cameraRight.z, 0.0f, -m_cameraRight.x);
	}
}
void Camera::key_callback(GLFWwindow *window, int key, int action)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
	{
		if (m_InUse)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_firstMouse = true;
		}
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_InUse = !m_InUse;
	}
}

// returns whether camera moved
bool Camera::move(GLFWwindow* window)
{
	bool activated = false;
	if (m_InUse)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (activated = true))
			m_cameraPos += m_speed * m_cameraForward;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && (activated = true))
			m_cameraPos -= m_speed * m_cameraForward;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (activated = true))
			m_cameraPos -= m_speed * m_cameraRight;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (activated = true))
			m_cameraPos += m_speed * m_cameraRight;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && (activated = true))
			m_cameraPos -= m_speed * glm::vec3(0.0f, 1.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (activated = true))
			m_cameraPos += m_speed * glm::vec3(0.0f, 1.0f, 0.0f);
		return activated;
	}
	else
		return false;

}

