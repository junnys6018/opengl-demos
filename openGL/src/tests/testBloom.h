#ifndef TEST_BLOOM
#define TEST_BLOOM

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>

class TestBloom : public Test
{
public:
	TestBloom(Camera& cam, GLFWwindow* win);
	~TestBloom();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;

	std::unique_ptr<Object> o_Sponza;
	std::unique_ptr<Shader> s_Bloom;

	int sWidth, sHeight;
	bool m_isWireFrame;
};
#endif