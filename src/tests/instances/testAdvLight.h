#ifndef TEST_ADV_LIGHT
#define TEST_ADV_LIGHT

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Utils/UniformBuffer.h"

#include <memory>
#include "glm/gtc/type_ptr.hpp"

class TestAdvLight : public Test
{
public:
	TestAdvLight(Base_Camera* cam, GLFWwindow* win);
	~TestAdvLight();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Base_Camera* m_camera;
	GLFWwindow* m_window;
	// Floor
	std::unique_ptr<VertexBuffer> floorVB;
	std::unique_ptr<VertexArray> floorVA;
	std::unique_ptr<Texture> t_floor;
	std::unique_ptr<Shader> s_blinnPhong;
	// Lamp
	std::unique_ptr<Object> o_sphere;
	std::unique_ptr<Shader> s_lamp;

	std::unique_ptr<UniformBuffer> u_matrix;

	int m_shadingMode, m_oldShadingMode;	// 0 == PHONG, 1 == BLINN_PHONG, 2 == BOTH
	int m_useGamma, m_oldUseGamma; // 0 == no, 1 == yes, 2 == both
	int sWidth, sHeight;
	float m_shininess;
	bool m_renderLamps;
};

class TestAdvLightDeployer : public Test_Deployer
{
public:
	TestAdvLightDeployer()
		: Test_Deployer("advLight")
	{

	}

	Test* Deploy(Base_Camera* cam, GLFWwindow* win) override
	{
		return new TestAdvLight(cam, win);
	}
};

#endif 
