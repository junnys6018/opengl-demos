#ifndef TEST_DIRECT_PBR
#define TEST_DIRECT_PBR
#include "../test.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"
#include "Timer.h"

#include <memory>

class TestDirectPBR : public Demo
{
public:
	TestDirectPBR(AbstractCamera* cam, GLFWwindow* win);
	~TestDirectPBR();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	void generateTextures(std::string name, std::string format);

	AbstractCamera* m_camera;
	GLFWwindow* m_Window;
	Timer timer;

	int sWidth, sHeight, renderMode, oldrenderMode;
	float metalness, roughness;
	bool paraMapping;

	glm::vec3 lightPos[4] = {
		glm::vec3(0.2f,  1.3f,  3.2f),
		glm::vec3(0.5f,  3.0f, -2.1f),
		glm::vec3(-0.1f, -1.0f,  2.1f),
		glm::vec3(-0.3f, -0.4f, -3.0f),
	};
	glm::vec3 lightColor[4] = {
		glm::vec3(0.070f , 21.31f, 0.01f ),
		glm::vec3(15.12f, 20.23f, 0.024f),
		glm::vec3(0.021f, 23.03f, 20.23f),
		glm::vec3(12.23f, 12.03f, 11.20f),
	};

	std::unique_ptr<Object> o_Sphere;
	std::unique_ptr<Object> o_Cylinder;
	std::unique_ptr<Object> o_Cube;

	std::unique_ptr<Shader> s_Shader;
	std::unique_ptr<Shader> s_Lamp;

	std::unique_ptr<Texture> t_Albedo;
	std::unique_ptr<Texture> t_Normal;
	std::unique_ptr<Texture> t_Metalness;
	std::unique_ptr<Texture> t_Roughness;
	std::unique_ptr<Texture> t_Displacement;
};

class TestDirectPBRDeployer : public DemoDeployer
{
public:
	TestDirectPBRDeployer()
		: DemoDeployer("Direct-PBR")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestDirectPBR(cam, win);
	}
};

#endif