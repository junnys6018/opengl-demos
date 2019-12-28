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
	TestManager(GLFWwindow** win);
	~TestManager();

	void gameLoop();
	Test* m_currentTest;
	Base_Camera* m_camera;
private:
	void registerTests();
	void OnImGuiRender(unsigned int fps);

	std::vector<Test_Deployer*> m_tests;
	bool show_controls_window;
	bool show_test_window;
	bool show_pos;
	int active_camera, old_active_camera;

	// Used to initalise tests
	GLFWwindow** m_window;
};

#endif