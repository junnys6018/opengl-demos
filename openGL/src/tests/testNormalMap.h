#ifndef TEST_NORMAL_MAP
#define TEST_NORMAL_MAP

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>

class TestNormMap : public Test
{
public:
	TestNormMap(Camera& cam, GLFWwindow* win);
	~TestNormMap();

	void OnUpdate() override;
	void OnImGuiRender() override;
	//void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;
	
	std::unique_ptr<VertexBuffer> quadVB;
	std::unique_ptr<VertexArray> quadVA;
	std::unique_ptr<Texture> t_DiffMap;
	std::unique_ptr<Texture> t_NormMap;

	std::unique_ptr<Shader> s_NormMap;

	bool useNormMap, visNormMap;
};
#endif