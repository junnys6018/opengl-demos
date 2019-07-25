#ifndef TEST_DIRECT_PBR
#define TEST_DIRECT_PBR
#include "../test.h"

#include <memory>

class TestDirectPBR : public Test
{
public:
	TestDirectPBR(Camera& cam, GLFWwindow* win);
	~TestDirectPBR();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;

};
#endif