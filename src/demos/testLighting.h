#ifndef TEST_LIGHTING_H
#define TEST_LIGHTING_H

#include "test.h"

#include "Shader.h"
#include "AbstractCamera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Texture.h"

#include <memory>
#include <string>

class TestLighting : public Demo
{
public:
	TestLighting(AbstractCamera*, GLFWwindow*);
	~TestLighting();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void key_callback(int key, int action) override;
private:
	GLFWwindow *m_Window;
	AbstractCamera* m_camera;

	std::unique_ptr<VertexBuffer> m_cubeBuf;
	std::unique_ptr<VertexArray> m_cubeVA;
	std::unique_ptr<VertexArray> m_lampVA;
	std::unique_ptr<Shader> m_lightingShader;
	std::unique_ptr<Shader> m_lampShader;
	std::unique_ptr<Texture> m_crateTex;

	float m_theta;
	bool m_isRotating;
	bool m_isSpotLighton;

	std::unique_ptr<Texture> m_diffMap;
	std::unique_ptr<Texture> m_specMap;
	std::unique_ptr<Texture> m_emisMap;

	glm::vec3 *m_cubePos;
	glm::vec3 *m_pointLightPositions;
	glm::vec3 m_dirLightColor;
};

class TestLightingDeployer : public DemoDeployer
{
public:
	TestLightingDeployer()
		: DemoDeployer("lighting")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestLighting(cam, win);
	}
};

#endif 