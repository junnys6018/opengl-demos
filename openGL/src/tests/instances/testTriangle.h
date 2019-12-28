#ifndef TEST_TRIANGLE_H
#define TEST_TRIANGLE_H

#include "test.h"
#include "Shader.h"

#include <memory>

class TestTriangle : public Test
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

class TestTriangleDeployer : public Test_Deployer
{
public:
	TestTriangleDeployer()
		: Test_Deployer("triangle")
	{

	}

	Test* Deploy(Base_Camera*, GLFWwindow*) override
	{
		return new TestTriangle();
	}
};

#endif // !TEST_TRIANGLE_H
