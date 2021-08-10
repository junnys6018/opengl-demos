#ifndef TEST_INSTANCING
#define TEST_INSTANCING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "CubeMap.h"
#include "Timer.h"

#include <memory>
#include <cstdlib> // for rand() and srand()

class TestInstancing : public Test
{
public:
	TestInstancing(Base_Camera* cam, GLFWwindow* win, uint16_t instances);
	~TestInstancing();

	void OnUpdate() override;
	void OnImGuiRender() override;
	//void framebuffer_size_callback(int width, int height) override;
private:
	Base_Camera* m_camera;
	GLFWwindow* m_window;
	Timer timer;
	std::unique_ptr<VertexArray> QuadVA;
	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<Shader> s_Quad;

	std::unique_ptr<Object> o_Planet;
	std::unique_ptr<Object> o_Rock;
	std::unique_ptr<Shader> s_planet;
	std::unique_ptr<Shader> s_rock;

	std::unique_ptr<CubeMap> m_skyBox;

	unsigned int offsetVBO, modelVBO;
	bool m_drawQuads;
	bool m_isWireFrame;
	const unsigned int m_amount;
};

class TestInstancingDeployer : public Test_Deployer
{
public:
	TestInstancingDeployer()
		: Test_Deployer("Instancing"), m_instances(10000)
	{

	}

	Test* Deploy(Base_Camera* cam, GLFWwindow* win) override
	{
		return new TestInstancing(cam, win, m_instances);
	}

	bool OnImguiUpdate() override
	{
		ImGui::Text("#instances:");
		ImGui::PushItemWidth(-1);
		ImGui::InputScalar("##Value", ImGuiDataType_U16, &m_instances);
		ImGui::PopItemWidth();
		return ImGui::Button("Enter");
	}

private:
	uint16_t m_instances;
};

#endif 
