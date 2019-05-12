#ifndef TEST_DEFERRED_SHADING
#define TEST_DEFERRED_SHADING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
//#include "Utils/UniformBuffer.h"

#include <memory>
//#include "glm/gtc/type_ptr.hpp"

class TestDeferred : public Test
{
public:
	TestDeferred(Camera& cam, GLFWwindow* win);
	~TestDeferred();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;

	std::unique_ptr<Shader> s_GeometryPass;
	std::unique_ptr<Shader> s_LightPass;

	std::unique_ptr<Object> o_Sponza;
	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<VertexArray> QuadVA;

	unsigned int gBuffer, gPosition, gNormal, gColorSpec, rboDepth;
	int sWidth, sHeight;
	int renderMode, old_renderMode;

	void genFrameBuffers();
};


#endif 
