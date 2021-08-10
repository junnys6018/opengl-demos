#ifndef TEST_CUBE_MAP
#define TEST_CUBE_MAP

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "AbstractCamera.h"

#include <memory>
#include "stb_image.h"

class TestCubeMap : public Demo
{
public:
	TestCubeMap(AbstractCamera* cam, GLFWwindow* win);
	~TestCubeMap();

	void OnUpdate() override;
private:
	void loadCubeMap(std::vector<std::string> faces);
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;

	std::unique_ptr<Object> oBlastoise;

	std::unique_ptr<Shader> skyBoxShader;
	std::unique_ptr<Shader> m_shader;

	std::unique_ptr<VertexBuffer> skyBoxVB;
	std::unique_ptr<VertexArray> skyBoxVA;
	
	unsigned int m_cubeMap;
};

class TestCubeMapDeployer : public DemoDeployer
{
public:
	TestCubeMapDeployer()
		: DemoDeployer("cubeMap")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestCubeMap(cam, win);
	}
};

#endif 
