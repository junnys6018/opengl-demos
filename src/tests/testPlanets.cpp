#include "testPlanets.h"
#include "debug.h"

TestPlanets::TestPlanets(Camera& cam, GLFWwindow* &win)
	:m_theta(0.0f), m_radii({ 2.0f, 4.0f, 7.0f, 10.0f, 15.0f }), m_size({ 0.6f, 1.0f, 0.9f, 1.8f, 0.2f })
	,m_camera(cam), m_isRotating(false), m_window(win), m_isWireFrame(false)
{
	// Setup cube vertices
	float cube[] = {
		//    POSITION		  TEX_COORD
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	m_cubeBuf = std::make_unique<VertexBuffer>(cube, sizeof(cube));
	m_cubeVA = std::unique_ptr<VertexArray>(new VertexArray(*m_cubeBuf, GL_FLOAT, { 3,2 }));

	m_sphere = std::make_unique<Object>("res/Objects/sphere.obj");
	m_sphereVA = std::unique_ptr<VertexArray>(new VertexArray(*(m_sphere->vertexBuffer), GL_FLOAT, { 3,3,2 }));
	m_sphereVA->addIndexBuffer(*(m_sphere->indexBuffer));

	m_simpleShader = std::make_unique<Shader>("res/Shaders/Simple.shader");
	m_cubeShader = std::make_unique<Shader>("res/shaders/Cube.shader");

	m_chris = std::make_unique<Texture>("res/Textures/Chris512.jpg");
	m_chris->Bind(0);
	m_cubeShader->setInt("Texture1", 0);
	GLCall(glClearColor(0.1f, 0.3f, 0.4f, 1.0f));
	GLCall(glEnable(GL_DEPTH_TEST));
}
TestPlanets::~TestPlanets()
{
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

void TestPlanets::OnUpdate()
{
	// camera movement
	m_camera.move(m_window);

	if (m_isRotating)
	{
		if (m_theta > 360.0 * 100.0)
			m_theta = 1.0f;
		else
			m_theta += 1.0f;
	}
	// MVP matrix
	glm::mat4 model_cube = glm::mat4(1.0f);
	model_cube = glm::rotate(model_cube, glm::radians(m_theta), glm::vec3(0.5f, 0.3f, 0.0f));

	glm::mat4 view = m_camera.getViewMatrix();

	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(width) / height, 0.1f, 100.0f);
	// Render
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_simpleShader->Use();
	m_sphereVA->Bind();
	for (int i = 0; i != 5; ++i)
	{
		glm::mat4 model_sphere = glm::mat4(1.0f);
		model_sphere = glm::rotate(model_sphere,	// follows kepler's law
			glm::radians(m_theta * 6.0f * sqrt(pow(1.0f / m_radii[i], 3))),
			glm::vec3(0.0f, 1.0f, 0.0f));
		model_sphere = glm::translate(model_sphere, glm::vec3(m_radii[i], 0.0f, 0.0f));
		model_sphere = glm::scale(model_sphere, glm::vec3(m_size[i]));
		m_simpleShader->setMat4("MVP", proj * view * model_sphere);
		GLCall(glDrawElements(GL_TRIANGLES, m_sphere->indexBuffer->getCount(), GL_UNSIGNED_INT, 0));
	}

	m_cubeShader->Use();
	m_cubeShader->setMat4("MVP", proj * view * model_cube);
	m_cubeVA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
}
void TestPlanets::key_callback(int key, int action)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		m_isRotating = !m_isRotating;
}
void TestPlanets::OnImGuiRender()
{
	ImGui::Checkbox("Rotate Planets", &m_isRotating);
	ImGui::SameLine();
	ImGui::Text("(key: r)");
	if (ImGui::Checkbox("Wireframe Mode", &m_isWireFrame))
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, (m_isWireFrame ? GL_LINE : GL_FILL)));
}