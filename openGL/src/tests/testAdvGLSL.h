#ifndef TEST_ADV_GLSL
#define TEST_ADV_GLSL

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>
#include "glm/gtc/type_ptr.hpp"

class TestAdvGLSL : public Test
{
public:
	TestAdvGLSL(Camera& cam, GLFWwindow* win);
	~TestAdvGLSL();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	Camera& m_camera;
	GLFWwindow* m_window;

	std::unique_ptr<Object> o_Cube;
	std::unique_ptr<Object> o_Blastoise;
	std::unique_ptr<Object> o_Nanosuit;

	std::unique_ptr<Shader> s_glPointSize;
	std::unique_ptr<Shader> s_glFragCoord;
	std::unique_ptr<Shader> s_glFrontFacing;
	std::unique_ptr<Shader> s_ExplodeShader;
	std::unique_ptr<Shader> s_NormVisualise;
	std::unique_ptr<Shader> s_Nanosuit;

	std::unique_ptr<Texture> m_back;
	std::unique_ptr<Texture> m_front;

	int sWidth, sHeight;
	unsigned int UBO;
	float m_time;
	bool m_isplaying; // true if m_time is oscillating
	bool m_isWireFrame;
	bool m_hasNormals;
};


#endif 
