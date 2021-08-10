#ifndef TEST_ADV_GLSL
#define TEST_ADV_GLSL

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "AbstractCamera.h"

#include <memory>
#include "glm/gtc/type_ptr.hpp"

class TestAdvGLSL : public Demo
{
public:
	TestAdvGLSL(AbstractCamera* cam, GLFWwindow* win);
	~TestAdvGLSL();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;

	std::unique_ptr<Object> o_Cube;
	std::unique_ptr<Object> o_Blastoise;
	std::unique_ptr<Object> o_Nanosuit;

	std::unique_ptr<Shader> s_glPointSize;
	std::unique_ptr<Shader> s_glFragCoord;
	std::unique_ptr<Shader> s_glFrontFacing;
	std::unique_ptr<Shader> s_ExplodeShader;
	std::unique_ptr<Shader> s_NormVisualise;
	std::unique_ptr<Shader> s_Nanosuit;

	std::unique_ptr<Texture> m_back;
	std::unique_ptr<Texture> m_front;

	int sWidth, sHeight;
	unsigned int UBO;
	float currTime, lastTime, timePoint;
	bool m_isplaying; // true if m_time is oscillating
	bool m_isWireFrame;
	bool m_hasNormals;
};

class TestAdvGLSLDeployer : public DemoDeployer
{
public:
	TestAdvGLSLDeployer()
		: DemoDeployer("advGLSL")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestAdvGLSL(cam, win);
	}
};

#endif 
