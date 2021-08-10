#ifndef TEST_PARALLAX_MAP
#define TEST_PARALLAX_MAP

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "AbstractCamera.h"

#include <memory>

class TestParaMap : public Demo
{
public:
	TestParaMap(AbstractCamera* cam, GLFWwindow* win);
	~TestParaMap();

	void OnUpdate() override;
	void OnImGuiRender() override;
private:
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;

	std::unique_ptr<VertexBuffer> quadVB;
	std::unique_ptr<VertexArray> quadVA;
	std::unique_ptr<Texture> t_Brick_DiffMap;
	std::unique_ptr<Texture> t_Brick_NormMap;
	std::unique_ptr<Texture> t_Brick_DispMap;
	std::unique_ptr<Texture> t_Wood_DiffMap;
	std::unique_ptr<Texture> t_Wood_NormMap;
	std::unique_ptr<Texture> t_Wood_DispMap;

	std::unique_ptr<Shader> s_ParaMap;
	int renderMode, oldrenderMode; // 0 - nothing, 1 - normal mapping, 2 - displacement mapping
};

class TestParaMapDeployer : public DemoDeployer
{
public:
	TestParaMapDeployer()
		: DemoDeployer("ParaMap")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestParaMap(cam, win);
	}
};

#endif