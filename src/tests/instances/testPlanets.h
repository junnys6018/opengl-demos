#ifndef TEST_PLANETS_H
#define TEST_PLANETS_H

#include "test.h"

#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Object.h"
#include "Texture.h"

#include <memory>
#include <initializer_list>
#include <vector>

class TestPlanets : public Test
{
public:
	TestPlanets(Base_Camera*, GLFWwindow*&);
	~TestPlanets();
public:
	void OnUpdate() override;
	void key_callback(int key, int action) override;
	void OnImGuiRender() override;
private:
	std::unique_ptr<VertexBuffer> m_cubeBuf;
	std::unique_ptr<VertexArray> m_cubeVA;

	std::unique_ptr<Object> m_sphere;

	std::unique_ptr<Shader> m_simpleShader;
	std::unique_ptr<Shader> m_cubeShader;

	std::unique_ptr<Texture> m_chris;

	float m_theta;
	std::vector<float> m_radii;
	std::vector<float> m_size;

	Base_Camera* m_camera;

	bool m_isRotating;
	bool m_isWireFrame;

	GLFWwindow* m_window;
};

class TestPlanetsDeployer : public Test_Deployer
{
public:
	TestPlanetsDeployer()
		: Test_Deployer("planets")
	{

	}

	Test* Deploy(Base_Camera* cam, GLFWwindow* win) override
	{
		return new TestPlanets(cam, win);
	}
};

#endif 
