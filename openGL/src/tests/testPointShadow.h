#ifndef TEST_POINT_SHADOWS
#define TEST_POINT_SHADOWS

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "Timer.h"

#include <memory>

class TestPointShadows : public Test
{
public:
	TestPointShadows(Base_Camera* cam, GLFWwindow* win);
	~TestPointShadows();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	void configureModelsAndDraw(Shader& shader, Draw_Flags flags);

	Base_Camera* m_camera;
	GLFWwindow* m_window;
	Timer timer[3];

	int sWidth, sHeight;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	bool visualiseDepthMap, movePointLight, snapToLight;
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	float currTime, lastTime, timePoint;
	// Objects
	std::unique_ptr<VertexBuffer> m_CubeVB;
	std::unique_ptr<VertexArray> m_CubeVA;
	std::unique_ptr<Texture> t_Cube;

	std::unique_ptr<Object> o_Sphere;
	std::unique_ptr<Object> o_Blastoise;
	std::unique_ptr<Object> o_Pikachu;
	std::unique_ptr<Object> o_Nidoqueen;
	std::unique_ptr<Object> o_Piplup;

	std::unique_ptr<Shader> s_ShadowPass;
	std::unique_ptr<Shader> s_RenderPass;
	std::unique_ptr<Shader> s_LampPass;

	glm::mat4 modelMatrices[6];

	unsigned int depthCubeMap, depthMapFBO;
};
#endif 
