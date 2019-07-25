#include "testIBL_PBR.h"
#include "debug.h"

TestIBL_PBR::TestIBL_PBR(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win)
{
}

TestIBL_PBR::~TestIBL_PBR()
{
}

void TestIBL_PBR::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void TestIBL_PBR::OnImGuiRender()
{
}

void TestIBL_PBR::framebuffer_size_callback(int width, int height)
{
}
