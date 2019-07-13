#ifndef TEST_SSAO
#define TEST_SSAO

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>
//#include "glm/gtc/type_ptr.hpp"

class TestSSAO : public Test
{
public:
	TestSSAO(Camera& cam, GLFWwindow* win);
	~TestSSAO();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;

	std::unique_ptr<Object> o_NanoSuit;
	std::unique_ptr<Object> o_Erato;

	std::unique_ptr<Shader> s_GeometryPass;
	std::unique_ptr<Shader> s_FinalPass;

	int sWidth, sHeight;
	unsigned int gBuffer, gPosition, gNormal, gColorSpec, rboDepth;
	bool m_isWireFrame;
};


#endif 
