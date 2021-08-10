#ifndef TEST_MODEL_LOADING
#define TEST_MODEL_LOADING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>

class TestFrameBuf : public Test
{
public:
	TestFrameBuf(Base_Camera* cam, GLFWwindow* win);
	~TestFrameBuf();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	bool GenFrameBuffer(int width, int height);
	GLFWwindow* m_window;
	Base_Camera* m_camera;

	std::unique_ptr<Object> oBlastoise;
	std::unique_ptr<Object> oPikachu;
	std::unique_ptr<Object> oNidoqueen;
	std::unique_ptr<Object> oPiplup;

	std::unique_ptr<VertexBuffer> FloorVB;
	std::unique_ptr<VertexArray> FloorVA;
	std::unique_ptr<Texture> FloorTex;

	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<VertexArray> QuadVA;

	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Shader> m_floorShader;
	std::unique_ptr<Shader> m_FramebufShader;

	bool m_isWireFrame;
	float m_camSpeed;
	float m_convOffset;

	int sWidth, sHeight;
	unsigned int FBO, RBO, texture;
	int flags, oldFlags;
};

class TestFrameBufDeployer : public Test_Deployer
{
public:
	TestFrameBufDeployer()
		: Test_Deployer("frameBuf")
	{

	}

	Test* Deploy(Base_Camera* cam, GLFWwindow* win) override
	{
		return new TestFrameBuf(cam, win);
	}
};

#endif
