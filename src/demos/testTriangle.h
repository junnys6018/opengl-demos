#ifndef TEST_TRIANGLE_H
#define TEST_TRIANGLE_H

#include "test.h"
#include "Shader.h"

#include <memory>

class TestTriangle : public Demo
{
public:
	TestTriangle();
	~TestTriangle();

	void OnUpdate() override;
	void OnImGuiRender() override;
private:
	unsigned int VBO;
	unsigned int VAO;
	
	std::unique_ptr<Shader> m_shader;

	glm::vec4 m_clearColor;
};

class TestTriangleDeployer : public DemoDeployer
{
public:
	TestTriangleDeployer()
		: DemoDeployer("triangle")
	{

	}

	Demo* Deploy(AbstractCamera*, GLFWwindow*) override
	{
		return new TestTriangle();
	}
};

#endif // !TEST_TRIANGLE_H
