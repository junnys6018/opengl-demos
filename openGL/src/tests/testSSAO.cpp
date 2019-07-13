#include "testSSAO.h"
#include "debug.h"

TestSSAO::TestSSAO(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_isWireFrame(false)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	o_NanoSuit = std::make_unique<Object>("res/Objects/nanosuit/nanosuit.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Erato = std::make_unique<Object>("res/Objects/erato/erato-1.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	s_FinalPass = std::make_unique<Shader>("res/shaders/Blastoise.shader");
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

}

TestSSAO::~TestSSAO()
{
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

void TestSSAO::OnUpdate()
{
	m_camera.move(m_window);
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));
	model = glm::translate(model, glm::vec3(11440.0f, 13000.0f, 12730.0f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);

	s_FinalPass->setMat4("MVP", proj * view * model);
	o_Erato->Draw(*s_FinalPass);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f));
	s_FinalPass->setMat4("MVP", proj * view * model);
	o_NanoSuit->Draw(*s_FinalPass);
}

void TestSSAO::OnImGuiRender()
{
	if (ImGui::Checkbox("Wireframe Mode", &m_isWireFrame))
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, (m_isWireFrame ? GL_LINE : GL_FILL)));
}

void TestSSAO::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
	}
}
