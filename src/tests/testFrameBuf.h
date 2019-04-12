#ifndef TEST_MODEL_LOADING
#define TEST_MODEL_LOADING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>

#include "imgui-master/imgui.h"

class TestFrameBuf : public Test
{
public:
	TestFrameBuf(Camera& cam, GLFWwindow* win);
	~TestFrameBuf();

	void OnUpdate() override;
	void OnImGuiRender() override;
private:
	GLFWwindow* m_window;
	Camera& m_camera;

	std::unique_ptr<Object> oBlastoise;
	std::unique_ptr<Object> oPikachu;

	std::unique_ptr<VertexBuffer> FloorVB;
	std::unique_ptr<VertexArray> FloorVA;
	std::unique_ptr<Texture> FloorTex;

	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Shader> m_floorShader;

	bool m_isWireFrame;
	float m_camSpeed;
};
#endif
