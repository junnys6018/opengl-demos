#ifndef CALLBACK_H
#define CALLBACK_H
#include <iostream>
#include <GLFW/glfw3.h>

void error_callback(int error, const char* description)
{
	std::cout << "[GLFW ERROR] " << error << " DESCRIPTION: " << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if (test_mgr.m_currentTest)
		test_mgr.m_currentTest->framebuffer_size_callback(width, height);
}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	test_mgr.m_camera->mouse_callback(xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_RELEASE)
	{
		GLFWmonitor* monitor = glfwGetWindowMonitor(window);
		if (monitor) // if fullscreen, turn to windowed mode
		{
			glfwSetWindowMonitor(window, NULL, 50, 50, 1080, 720, GLFW_DONT_CARE);
			glfwSwapInterval(1); // enable vsync
		}
		else // make it fullscreen
		{
			GLFWmonitor* primary = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primary);
			glfwSetWindowMonitor(window, primary, 0, 0, mode->width, mode->height, mode->refreshRate);
			glfwSwapInterval(1); // enable vsync
		}
	}
	else if (test_mgr.m_currentTest)
		test_mgr.m_currentTest->key_callback(key, action);
	test_mgr.m_camera->key_callback(window, key, action);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	test_mgr.m_camera->scroll_callback(window, xoffset, yoffset);
}

void printBasicInfo()
{
	std::cout << "HARDWARE INFO: "     <<                                 std::endl;
	std::cout << "OpenGl version: "    << glGetString(GL_VERSION)      << std::endl;
	std::cout << "GLFW version: "      << glfwGetVersionString()       << std::endl;
	std::cout << "Renderer: "          << glGetString(GL_RENDERER)     << std::endl;
	std::cout << "Vendor: "            << glGetString(GL_VENDOR)       << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	int numAttributes;
	GLCall(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes));
	std::cout << "Maximum # of vertex attributes supported: " << numAttributes << '\n';

	int work_grp_cnt[3];
	GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]));
	GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]));
	GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]));
	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];
	GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]));
	GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]));
	GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]));
	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	std::cout << "-------------------------------------------------------------" << std::endl;
}
#include "imgui.h"

static void (*PrevWindowCreateCallback)(ImGuiViewport* viewPort);
void OnWindowCreate(ImGuiViewport* viewPort)
{
	PrevWindowCreateCallback(viewPort);

	static int width, height, nrChannels;
	static unsigned char* img = stbi_load("res/icon.png", &width, &height, &nrChannels, 4);
	static GLFWimage icon = { width,height,img };
	glfwSetWindowIcon(*(GLFWwindow**)(viewPort->PlatformUserData), 1, &icon);
}
#endif
