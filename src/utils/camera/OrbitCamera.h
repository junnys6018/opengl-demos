#pragma once
#include "AbstractCamera.h"

class OrbitCamera : public AbstractCamera
{
public:
	OrbitCamera();
	~OrbitCamera();

	virtual void resetPos() override;
	//virtual void key_callback(GLFWwindow* window, int key, int action) override;
	virtual void mouse_callback(double xpos, double ypos) override;
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) override;
	virtual bool handleWindowInput(GLFWwindow* window) override;
private:
	void updateVectors();
	float m_yaw, m_pitch, m_radius;
	float m_lastX, m_lastY;
	float m_sensitivity, m_scroll_sensitivity;

	bool m_mouseHeld;
};