#ifndef TEST_SHADOWS
#define TEST_SHADOWS

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "Timer.h"

#include <memory>
#include "glm/gtc/type_ptr.hpp"

class TestShadows : public Test
{
public:
	TestShadows(Camera& cam, GLFWwindow* win);
	~TestShadows();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;
	Timer timer[3];

	int sWidth, sHeight;
	static const int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	// Objects
	std::unique_ptr<VertexBuffer> m_floorVB;
	std::unique_ptr<VertexArray> m_floorVA;
	std::unique_ptr<Texture> t_floor;

	std::unique_ptr<Object> o_sphere;
	std::unique_ptr<VertexBuffer> m_cubeVB;
	std::unique_ptr<VertexArray> m_cubeVA;
	std::unique_ptr<Texture> t_cube;

	std::unique_ptr<VertexBuffer> m_quadVB;
	std::unique_ptr<VertexArray> m_quadVA;

	std::unique_ptr<Shader> s_shadowPass;
	std::unique_ptr<Shader> s_renderPass;
	std::unique_ptr<Shader> s_depthMap;

	unsigned int depthMapFBO;
	unsigned int depthMap;

	std::unique_ptr<UniformBuffer> u_matrix;
	std::vector<glm::mat4> modelMats;

	bool renderDepthMap;
	float m_lightAngle;

	void shadowPass();
	void renderPass();
	void calcLightFromAngle();
};
#endif 
