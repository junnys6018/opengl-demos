#ifndef TEST_NORMAL_MAP
#define TEST_NORMAL_MAP

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "AbstractCamera.h"

#include <memory>

class TestNormMap : public Demo
{
public:
	TestNormMap(AbstractCamera* cam, GLFWwindow* win);
	~TestNormMap();

	void OnUpdate() override;
	void OnImGuiRender() override;
private:
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;
	
	std::unique_ptr<VertexBuffer> quadVB;
	std::unique_ptr<VertexArray> quadVA;
	std::unique_ptr<Texture> t_DiffMap;
	std::unique_ptr<Texture> t_NormMap;

	std::unique_ptr<Object> o_NanoSuit;
	std::unique_ptr<Object> o_sphere;

	std::unique_ptr<Shader> s_NormMap;
	std::unique_ptr<Shader> s_lamp;

	bool useNormMap, visNormMap, drawLamp;
};

class TestNormMapDeployer : public DemoDeployer
{
public:
	TestNormMapDeployer()
		: DemoDeployer("NormMap")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestNormMap(cam, win);
	}
};

#endif