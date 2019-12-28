#ifndef TEST_H
#define TEST_H

#include "imgui.h"
#define GLEW_STATIC
#include "GL/glew.h"

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


#endif