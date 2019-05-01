#ifndef TEST_H
#define TEST_H
#include <vector>
#include <functional>
#include <string>
#include <sstream>

#include "imgui-master/imgui.h"
#include <iostream> // remove after temp
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

class TestManager
{
public:
	TestManager();
	~TestManager();

	void parseInput(std::string);
	void registerTest(std::string, std::function<Test*()>);

	void OnImGuiRender();

	Test* m_currentTest;
private:
	std::vector<std::pair<std::string, std::function<Test*()>>> m_tests;
	bool show_demo_window;
};

#endif 