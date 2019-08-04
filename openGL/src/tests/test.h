#ifndef TEST_H
#define TEST_H
#include <vector>
#include <functional>
#include <string>
#include <sstream>

#include "imgui.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Camera.h"
class Test
{
public:
	Test() {}
	virtual ~Test() {}

	virtual void OnUpdate() {}
	virtual void OnImGuiRender() { ImGui::Text("Hello World!"); };
	virtual void key_callback(int key, int action) {}
	virtual void framebuffer_size_callback(int width, int height) {}
};
// TODO: refactor TestManager
class TestManager
{
public:
	TestManager(GLFWwindow** win);
	~TestManager();

	void parseInput(std::string);
	void registerTest(std::string, std::function<Test* (Base_Camera*, GLFWwindow*)>, std::function<bool()>);
	void registerTests();

	void gameLoop();
	Test* m_currentTest;
	Base_Camera* m_camera;
private:
	void OnImGuiRender(unsigned int fps);

	std::vector<std::tuple<std::string, std::function<Test* (Base_Camera*, GLFWwindow*)>, std::function<bool()>>> m_tests;
	bool show_controls_window;
	bool show_test_window;
	int active_camera, old_active_camera;

	// Used to initalise tests
	GLFWwindow** m_window;
};

#endif