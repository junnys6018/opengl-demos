#ifndef TEST_DEFERRED_SHADING
#define TEST_DEFERRED_SHADING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "AbstractCamera.h"
#include "UniformBuffer.h"
#include "Timer.h"

#include <memory>
#include <stdlib.h> // srand() and rand()
#include <algorithm> // std::max()
#include "glm/gtc/type_ptr.hpp"

struct PointLight;

void genLightAttibute(std::vector<PointLight>& vecLight, glm::vec4 position);

class TestDeferred : public Demo
{
public:
	TestDeferred(AbstractCamera* cam, GLFWwindow* win, uint16_t nr_lights);
	~TestDeferred();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;
	Timer timer[3];

	std::unique_ptr<Shader> s_GeometryPass;
	std::unique_ptr<Shader> s_LightPass;
	std::unique_ptr<Shader> s_Lamp;
	std::unique_ptr<Shader> s_FustrumCull;
	std::unique_ptr<Shader> s_FinalPass;

	std::unique_ptr<Object> o_Sponza;
	std::unique_ptr<Object> o_Cube;
	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<VertexArray> QuadVA;

	std::unique_ptr<UniformBuffer> u_Matrix;

	unsigned int gBuffer, gPosition, gNormal, gColorSpec, rboDepth;
	unsigned int lightsSSBO;
	unsigned int modelVBO;
	unsigned int tex_output;

	int sWidth, sHeight;
	int renderMode, old_renderMode, computeTarget, oldComputeTarget;
	bool renderLights;
	bool useTileBased;
	const unsigned int NR_LIGHTS;
	float exposure;

	void genFrameBuffers();
	void genLightsInCylinder(std::vector<PointLight>& vecLight, float RADIUS, float HEIGHT);
	void genLightsInRect(std::vector<PointLight>& vecLight, glm::vec3 dimensions, glm::vec3 center);
};

class TestDeferredDeployer : public DemoDeployer
{
public:
	TestDeferredDeployer()
		: DemoDeployer("Deferred"), m_nr_lights(32)
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestDeferred(cam, win, m_nr_lights);
	}

	bool OnImguiUpdate() override
	{
		ImGui::Text("# Lights");
		ImGui::PushItemWidth(-1);
		ImGui::InputScalar("##Value", ImGuiDataType_U16, &m_nr_lights);
		ImGui::PopItemWidth();
		return ImGui::Button("Enter");
	}

private:
	uint16_t m_nr_lights;
};

#endif 
