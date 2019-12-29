#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <tuple>

#include "../tests/__Test__.h"

#include "GLFW/glfw3.h"

class TestManager
{
public:
	TestManager();
	void init(GLFWwindow* win);

	~TestManager();

	void gameLoop();
	Test* m_currentTest;
	Base_Camera* m_camera;
private:
	void registerTests();
	void OnImGuiRender(unsigned int fps);

	std::vector<Test_Deployer*> m_tests;

	// Imgui members
	bool show_controls_window;
	bool show_test_window;
	bool show_pos;
	int active_camera, old_active_camera;

	GLFWwindow* m_window;
};

#endif