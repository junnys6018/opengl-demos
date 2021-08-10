#ifndef TEST_PLANETS_H
#define TEST_PLANETS_H

#include "test.h"

#include "Shader.h"
#include "AbstractCamera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Object.h"
#include "Texture.h"

#include <memory>
#include <initializer_list>
#include <vector>

class TestPlanets : public Demo
{
public:
	TestPlanets(AbstractCamera*, GLFWwindow*&);
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

	AbstractCamera* m_camera;

	bool m_isRotating;
	bool m_isWireFrame;

	GLFWwindow* m_Window;
};

class TestPlanetsDeployer : public DemoDeployer
{
public:
	TestPlanetsDeployer()
		: DemoDeployer("planets")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestPlanets(cam, win);
	}
};

#endif 
