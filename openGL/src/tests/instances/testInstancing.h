#ifndef TEST_INSTANCING
#define TEST_INSTANCING

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "CubeMap.h"
#include "Timer.h"

#include <memory>
#include <cstdlib> // for rand() and srand()

class TestInstancing : public Test
{
public:
	TestInstancing(Base_Camera* cam, GLFWwindow* win, uint16_t instances);
	~TestInstancing();

	void OnUpdate() override;
	void OnImGuiRender() override;
	//void framebuffer_size_callback(int width, int height) override;
private:
	Base_Camera* m_camera;
	GLFWwindow* m_window;
	Timer timer;
	std::unique_ptr<VertexArray> QuadVA;
	std::unique_ptr<VertexBuffer> QuadVB;
	std::unique_ptr<Shader> s_Quad;

	std::unique_ptr<Object> o_Planet;
	std::unique_ptr<Object> o_Rock;
	std::unique_ptr<Shader> s_planet;
	std::unique_ptr<Shader> s_rock;

	std::unique_ptr<CubeMap> m_skyBox;

	unsigned int offsetVBO, modelVBO;
	bool m_drawQuads;
	bool m_isWireFrame;
	const unsigned int m_amount;
};


#endif 