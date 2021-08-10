#include "Camera.h"

#include <iostream>

Base_Camera::Base_Camera()
	: m_FOV(45.0f), m_speed(5.0f), m_firstMouse(true), m_InUse(false)
{
	resetPos();
}
Base_Camera::~Base_Camera()
{
}
glm::vec3 Base_Camera::getCameraPos() const
{
	return m_cameraPos;
}
glm::vec3 Base_Camera::getCameraDir() const
{
	return m_cameraDir;
}
glm::vec3 Base_Camera::getCameraRight() const
{
	return m_cameraRight;
}
glm::vec3 Base_Camera::getCameraForward() const
{
	return m_cameraForward;
}
glm::mat4 Base_Camera::getViewMatrix() const
{
	return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f));
}
float Base_Camera::getFOV() const
{
	return m_FOV;
}
void Base_Camera::setSpeed(const float speed)
{
	m_speed = speed;
}
void Base_Camera::setPos(const glm::vec3 pos)
{
	m_cameraPos = pos;
}