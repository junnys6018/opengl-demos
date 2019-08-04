#ifndef TEST_IBL_PBR
#define TEST_IBL_PBR
#include "../test.h"

#include <memory>

class TestIBL_PBR : public Test
{
public:
	TestIBL_PBR(Base_Camera* cam, GLFWwindow* win);
	~TestIBL_PBR();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Base_Camera* m_camera;
	GLFWwindow* m_window;

};
#endif
