#ifndef TEST_H
#define TEST_H

#include "imgui.h"
#define GLEW_STATIC
#include "GL/glew.h"

#include <string>

#include "Camera.h"

class Test
{
public:
	virtual void OnUpdate() {}
	virtual void OnImGuiRender() { ImGui::Text("Hello World!"); };
	virtual void key_callback(int key, int action) {}
	virtual void framebuffer_size_callback(int width, int height) {}
};

class Test_Deployer
{
public:
	const char* getName() const { return m_name; }

	virtual Test* Deploy(Base_Camera*, GLFWwindow*) { return new Test(); }
	virtual bool OnImguiUpdate() { return true; }

protected:
	Test_Deployer(const char* name): m_name(name) {}

private:
	const char* m_name;
};

#endif