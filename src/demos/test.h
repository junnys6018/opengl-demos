#pragma once
#include <imgui.h>
#include <glad/glad.h>

#include "AbstractCamera.h"

class Demo
{
public:
	virtual void OnUpdate() {}
	virtual void OnImGuiRender() { ImGui::Text("Hello World!"); };
	virtual void key_callback(int key, int action) {}
	virtual void framebuffer_size_callback(int width, int height) {}
};

class DemoDeployer
{
public:
	const char* getName() const { return m_name; }

	virtual Demo* Deploy(AbstractCamera*, GLFWwindow*) { return new Demo(); }
	virtual bool OnImguiUpdate() { return true; }

protected:
	DemoDeployer(const char* name): m_name(name) {}

private:
	const char* m_name;
};