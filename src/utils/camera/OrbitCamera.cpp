#include "OrbitCamera.h"

OrbitCamera::OrbitCamera()
	:m_yaw(270.0f), m_pitch(0.0f), m_radius(1.0f), m_lastX(0.0f), m_lastY(0.0f), m_sensitivity(0.3f), m_scroll_sensitivity(0.4f)
{
	resetPos();
}

OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::resetPos()
{
	m_radius = 0.01f;
	updateVectors();
}

void OrbitCamera::mouse_callback(double xpos, double ypos)
{
	if (m_mouseHeld)
	{
		if (m_firstMouse) // this bool variable is initially set to true
		{
			m_lastX = xpos;
			m_lastY = ypos;
			m_firstMouse = false;
		}
		float xoffset = xpos - m_lastX;
		float yoffset = ypos - m_lastY;
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
		
		updateVectors();
	}
}

void OrbitCamera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_radius -= m_scroll_sensitivity * yoffset;
	if (m_radius < 0.1f)
		m_radius = 0.1f;
	updateVectors();
}

bool OrbitCamera::handleWindowInput(GLFWwindow* window)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS && !m_mouseHeld) // if first press
	{
		m_mouseHeld = true;
		m_firstMouse = true;
	}
	else if (state == GLFW_RELEASE)
	{
		m_mouseHeld = false;
	}
	return true; // FIX-ME return true if mouse is held or scroll wheel is in use
}

void OrbitCamera::updateVectors()
{
	m_cameraDir.x = -cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	m_cameraDir.y = -sin(glm::radians(m_pitch));
	m_cameraDir.z = -cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

	m_cameraPos = -m_cameraDir * m_radius;

	m_cameraRight = glm::normalize(glm::cross(m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_cameraForward = glm::vec3(m_cameraRight.z, 0.0f, -m_cameraRight.x);
}
