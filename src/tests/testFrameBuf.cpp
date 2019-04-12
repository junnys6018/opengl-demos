#include "testFrameBuf.h"
#include "debug.h"

TestFrameBuf::TestFrameBuf(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_isWireFrame(false), m_camSpeed(1.0f)
{
	oBlastoise = std::make_unique<Object>("res/Objects/Pokemon/Blastoise/Blastoise.obj");
	oPikachu = std::make_unique<Object>("res/Objects/Pokemon/Pikachu/Pikachu.obj");
	m_shader = std::make_unique<Shader>("res/Shaders/Blastoise.shader");
	m_floorShader = std::make_unique<Shader>("res/Shaders/Cube.shader");

	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.01f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.01f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.01f, -5.0f,  0.0f, 1.0f,

		 5.0f, -0.01f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.01f, -5.0f,  0.0f, 1.0f,
		 5.0f, -0.01f, -5.0f,  1.0f, 1.0f
	};
	FloorVB = std::make_unique<VertexBuffer>(planeVertices, sizeof(planeVertices));
	FloorVA = std::unique_ptr<VertexArray>(new VertexArray(*FloorVB, GL_FLOAT, { 3,2 }));
	FloorTex = std::make_unique<Texture>("res/Textures/battleground.jpg");

	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glEnable(GL_DEPTH_TEST));
}

TestFrameBuf::~TestFrameBuf()
{
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	m_camera.setSpeed(0.1f);
}

void TestFrameBuf::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_camera.move(m_window);

	// Render
	glm::mat4 view = m_camera.getViewMatrix();

	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(width) / height, 0.1f, 100.0f);
	// Floor
	FloorVA->Bind();
	FloorTex->Bind(0);
	m_floorShader->setInt("Texture1", 0);
	m_floorShader->setMat4("MVP", proj * view);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	// Blastoise
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
	model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
	m_shader->setMat4("MVP", proj * view * model);
	oBlastoise->Draw(*m_shader);
	// Pikachu
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
	model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_shader->setMat4("MVP", proj * view * model);
	oPikachu->Draw(*m_shader);

}

void TestFrameBuf::OnImGuiRender()
{
	if (ImGui::Checkbox("Wireframe Mode", &m_isWireFrame))
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, (m_isWireFrame ? GL_LINE : GL_FILL)));
	if (ImGui::SliderFloat("Cam Speed", &m_camSpeed, 0.0f, 3.0f))
		m_camera.setSpeed(m_camSpeed / 10.0f);
}
