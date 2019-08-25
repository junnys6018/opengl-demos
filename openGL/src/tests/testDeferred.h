#ifndef TEST_DEFERRED_SHADING
#define TEST_DEFERRED_SHADING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "Timer.h"

#include <memory>
#include <stdlib.h> // srand() and rand()
#include <algorithm> // std::max()
#include "glm/gtc/type_ptr.hpp"

struct PointLight;

void genLightAttibute(std::vector<PointLight>& vecLight, glm::vec4 position);

class TestDeferred : public Test
{
public:
	TestDeferred(Base_Camera* cam, GLFWwindow* win, uint16_t nr_lights);
	~TestDeferred();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Base_Camera* m_camera;
	GLFWwindow* m_window;
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


#endif 
