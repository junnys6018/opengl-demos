#pragma once

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "AbstractCamera.h"
#include "Timer.h"

class TestSSAO : public Demo
{
public:
	TestSSAO(AbstractCamera* cam, GLFWwindow* win);
	~TestSSAO();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	void genFrameBuffers();
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;
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

class TestSSAODeployer : public DemoDeployer
{
public:
	TestSSAODeployer()
		: DemoDeployer("SSAO")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestSSAO(cam, win);
	}
};
