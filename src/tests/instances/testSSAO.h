#ifndef TEST_SSAO
#define TEST_SSAO

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Timer.h"

#include <memory>
//#include "glm/gtc/type_ptr.hpp"

class TestSSAO : public Test
{
public:
	TestSSAO(Base_Camera* cam, GLFWwindow* win);
	~TestSSAO();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	void genFrameBuffers();
	Base_Camera* m_camera;
	GLFWwindow* m_window;
	Timer timer[5];

	std::unique_ptr<Object> o_NanoSuit;
	std::unique_ptr<Object> o_Pikachu;

	std::unique_ptr<Shader> s_GeometryPass;
	std::unique_ptr<Shader> s_ssaoPass;
	std::unique_ptr<Shader> s_BlurPass;
	std::unique_ptr<Shader> s_LightingPass;
	std::unique_ptr<Shader> s_LampPass;

	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<VertexArray> QuadVA;

	std::unique_ptr<VertexBuffer> CubeVB;
	std::unique_ptr<VertexArray> CubeVA;
	std::unique_ptr<Texture> CubeTexture;

	int sWidth, sHeight;
	unsigned int gBuffer, gPosition, gNormal, gColor, rboDepth, noiseTexture, ssaoFBO, ssaoColor, ssaoBlurFBO, ssaoBlurColor;
	int renderMode, oldRenderMode;
	glm::vec3 LightColor, LightViewPos;
	float power;
};

class TestSSAODeployer : public Test_Deployer
{
public:
	TestSSAODeployer()
		: Test_Deployer("SSAO")
	{

	}

	Test* Deploy(Base_Camera* cam, GLFWwindow* win) override
	{
		return new TestSSAO(cam, win);
	}
};

#endif 
