#include "AbstractCamera.h"

#include <iostream>

AbstractCamera::AbstractCamera()
	: m_FOV(45.0f), m_speed(5.0f), m_firstMouse(true), m_InUse(false)
{
	resetPos();
}
AbstractCamera::~AbstractCamera()
{
}
glm::vec3 AbstractCamera::getCameraPos() const
{
	return m_cameraPos;
}
glm::vec3 AbstractCamera::getCameraDir() const
{
	return m_cameraDir;
}
glm::vec3 AbstractCamera::getCameraRight() const
{
	return m_cameraRight;
}
glm::vec3 AbstractCamera::getCameraForward() const
{
	return m_cameraForward;
}
glm::mat4 AbstractCamera::getViewMatrix() const
{
	return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDir, glm::vec3(0.0f, 1.0f, 0.0f));
}
float AbstractCamera::getFOV() const
{
	return m_FOV;
}
void AbstractCamera::setSpeed(const float speed)
{
	m_speed = speed;
}
void AbstractCamera::setPos(const glm::vec3 pos)
{
	m_cameraPos = pos;
}