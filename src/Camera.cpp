#include "Camera.h"

Camera::Camera()
	:m_yaw(270.0f), m_pitch(0.0f), m_lastX(0.0f), m_lastY(0.0f)
	, m_sensitivity(0.07f), m_speed(0.1f), m_firstMouse(true), m_hasCursor(true)
{
	m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);	// Position in worldspace
	m_cameraDir = glm::vec3(0.0f, 0.0f, -1.0f);	// direction the camera is facing relative to world space
	m_cameraRight = glm::normalize(glm::cross(m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_cameraForward = glm::vec3(m_cameraRight.z, 0.0f, -m_cameraRight.x);
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

void Camera::mouse_callback(double xpos, double ypos)
{
	if (!m_hasCursor)	// if GLFW_CURSOR_DISABLED is set
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
		float sensitivity = 0.1f;	
		xoffset *= sensitivity;
		yoffset *= sensitivity;
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
		if (m_hasCursor)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_firstMouse = true;
		}
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_hasCursor = !m_hasCursor;
	}
}

void Camera::move(int dir)
{
	switch (dir)
	{
	case GLFW_KEY_W:
		m_cameraPos += m_speed * m_cameraForward;
		break;
	case GLFW_KEY_A:
		m_cameraPos -= m_speed * m_cameraRight;
		break;
	case GLFW_KEY_S:
		m_cameraPos -= m_speed * m_cameraForward;
		break;
	case GLFW_KEY_D:
		m_cameraPos += m_speed * m_cameraRight;
		break;
	}
}