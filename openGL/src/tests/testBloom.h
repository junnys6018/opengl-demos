#ifndef TEST_BLOOM
#define TEST_BLOOM

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Utils/UniformBuffer.h"

#include <memory>
#include "glm/gtc/type_ptr.hpp"

class TestBloom : public Test
{
public:
	TestBloom(Camera& cam, GLFWwindow* win);
	~TestBloom();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;

	std::unique_ptr<Shader> s_Bloom;
	std::unique_ptr<Shader> s_Lamp;
	std::unique_ptr<Shader> s_Blur;
	std::unique_ptr<Shader> s_Final;

	std::unique_ptr<Object> o_Sponza;
	std::unique_ptr<Object> o_Cube;

	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<VertexArray> QuadVA;

	std::unique_ptr<UniformBuffer> u_Matrices;
	std::unique_ptr<UniformBuffer> u_Lights;

	unsigned int hdrFBO, depthRBO;
	unsigned int colorBuffers[2];

	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];

	int sWidth, sHeight;
	bool m_isWireFrame;
	// Shader Uniforms
	float exposure;
	int renderMode, old_renderMode;


	void genFrameBuffers();
};
#endif