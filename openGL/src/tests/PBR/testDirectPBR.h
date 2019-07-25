#ifndef TEST_DIRECT_PBR
#define TEST_DIRECT_PBR
#include "../test.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"
#include "Timer.h"

#include <memory>

class TestDirectPBR : public Test
{
public:
	TestDirectPBR(Camera& cam, GLFWwindow* win);
	~TestDirectPBR();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;
	Timer timer;

	int sWidth, sHeight, renderMode, oldrenderMode;
	float metalness, roughness;

	glm::vec3 lightPos[4] = {
		glm::vec3(0.2f,  1.3f,  3.2f),
		glm::vec3(0.5f,  3.0f, -2.1f),
		glm::vec3(-0.1f, -1.0f,  2.1f),
		glm::vec3(-0.3f, -0.4f, -3.0f),
	};
	glm::vec3 lightColor[4] = {
		glm::vec3(23.47f, 21.31f, 20.79f),
		glm::vec3(15.12f, 20.23f, 2.024f),
		glm::vec3(4.021f, 23.03f, 20.23f),
		glm::vec3(10.23f, 12.03f, 11.20f),
	};

	std::unique_ptr<Object> o_Sphere;
	std::unique_ptr<Object> o_Cube;
	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<VertexArray> QuadVA;

	std::unique_ptr<Shader> s_Shader;
	std::unique_ptr<Shader> s_Lamp;

	std::unique_ptr<Texture> t_Albedo;
	std::unique_ptr<Texture> t_Normal;
	std::unique_ptr<Texture> t_Metalness;
	std::unique_ptr<Texture> t_Roughness;

};
#endif