#include "testDirectPBR.h"
#include "debug.h"

TestDirectPBR::TestDirectPBR(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win)
{
}

TestDirectPBR::~TestDirectPBR()
{
}

void TestDirectPBR::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void TestDirectPBR::OnImGuiRender()
{
}

void TestDirectPBR::framebuffer_size_callback(int width, int height)
{
}
