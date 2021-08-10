#ifndef WALK_CAMERA_H
#define WALK_CAMERA_H

#include "Camera.h"

class Walk_Camera : public Base_Camera
{
public:
	Walk_Camera();
	~Walk_Camera();

	virtual void resetPos() override;
	virtual void key_callback(GLFWwindow* window, int key, int action) override;
	virtual void mouse_callback(double xpos, double ypos) override;
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) override;
	virtual bool handleWindowInput(GLFWwindow* window) override;
private:
	float m_yaw, m_pitch;
	float m_lastX, m_lastY;
	float m_sensitivity;

	float prevtime, currtime, delta;
};
#endif // !WALK_CAMERA_H
