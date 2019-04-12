#ifndef TEST_TRIANGLE_H
#define TEST_TRIANGLE_H

#include "test.h"
#include "Shader.h"

#include <memory>

#include "imgui-master/imgui.h"

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
#endif // !TEST_TRIANGLE_H
