#include "testBloom.h"
#include "debug.h"

TestBloom::TestBloom(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_isWireFrame(false)
{
	glfwGetWindowSize(m_window, &sWidth, &sHeight);
	o_Sponza = std::make_unique<Object>("res/Objects/sponza/sponza.obj.expanded");
	s_Bloom = std::make_unique<Shader>("res/Shaders/Bloom.shader");

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_CULL_FACE));
}

TestBloom::~TestBloom()
{
	GLCall(glDisable(GL_CULL_FACE));
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

void TestBloom::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	m_camera.move(m_window);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.015f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);

	s_Bloom->setMat4("MVP", proj * view * model);
	o_Sponza->Draw(*s_Bloom);
}

void TestBloom::OnImGuiRender()
{
	if (ImGui::Checkbox("WireFrame", &m_isWireFrame))
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, m_isWireFrame ? GL_LINE : GL_FILL));
}

void TestBloom::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
	}
}
