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
	TestManager(Camera& cam, GLFWwindow** win);
	~TestManager();

	void parseInput(std::string);
	void registerTest(std::string, std::function<Test* (Camera&, GLFWwindow*)>, std::function<bool()>);
	void registerTests();

	void gameLoop();
	Test* m_currentTest;
private:
	void OnImGuiRender(unsigned int fps);

	std::vector<std::tuple<std::string, std::function<Test* (Camera&, GLFWwindow*)>, std::function<bool()>>> m_tests;
	bool show_demo_window;
	bool show_overlay;

	// Used to initalise tests
	Camera& m_camera;
	GLFWwindow** m_window;
};

#endif