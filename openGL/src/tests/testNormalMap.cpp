#include "testNormalMap.h"
#include "debug.h"

TestNormMap::TestNormMap(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), useNormMap(false), visNormMap(false), drawLamp(true)
{
	float quadVertices[] = {
		// positions        // Normals       // texture Coords
		 2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
		-2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

		 2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		 2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f
	};
	float tangents[] = {
		// w component speciefies bitangent direction
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};
	quadVB = std::make_unique<VertexBuffer>(sizeof(quadVertices) + sizeof(tangents));
	quadVA = std::unique_ptr<VertexArray>(new VertexArray(*quadVB, GL_FLOAT, { 3,3,2 }));
	quadVA->Bind();
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, sizeof(quadVertices), sizeof(tangents), tangents));
	GLCall(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(quadVertices))));
	GLCall(glEnableVertexAttribArray(3));
	quadVA->unBind();

	t_DiffMap = std::make_unique<Texture>("res/Textures/brickwall.jpg");
	t_NormMap = std::make_unique<Texture>("res/Textures/brickwall_normal.jpg");

	o_NanoSuit = std::make_unique<Object>("res/Objects/nanosuit/nanosuit.obj.expanded", OBJECT_INIT_FLAGS_GEN_TANGENT | OBJECT_INIT_FLAGS_GEN_NORMAL | OBJECT_INIT_FLAGS_GEN_TEXTURE);

	s_NormMap = std::make_unique<Shader>("res/Shaders/NormMap.shader");

	s_NormMap->setVec3("light.position", glm::vec3(0.0f, 1.0f, 1.0f));
	s_NormMap->setFloat("light.constant", 1.0f);
	s_NormMap->setFloat("light.lin", 0.09f);
	s_NormMap->setFloat("light.quadratic", 0.032f);

	s_NormMap->setInt("Texture1", 0);
	s_NormMap->setInt("NormMap", 1);
	// Lamp
	o_sphere = std::make_unique<Object>("res/Objects/sphere.obj");
	s_lamp = std::make_unique<Shader>("res/Shaders/lamp.shader");

	GLCall(glEnable(GL_DEPTH_TEST));
}

TestNormMap::~TestNormMap()
{
}

void TestNormMap::OnUpdate()
{
	if (m_camera.move(m_window))
		s_NormMap->setVec3("camPos", m_camera.getCameraPos());
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(m_camera.getFOV()), (float)(width) / height, 0.1f, 100.0f);
	s_NormMap->setMat4("model", model);
	s_NormMap->setMat4("view", view);
	s_NormMap->setMat4("proj", proj);
	// Wall
	quadVA->Bind();
	t_DiffMap->Bind(0);
	t_NormMap->Bind(1);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	// Nanosuit
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(5.0f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));
	s_NormMap->setMat4("model", model);
	o_NanoSuit->Draw(*s_NormMap, DRAW_FLAGS_DIFFUSE | DRAW_FLAGS_NORMAL);
	// Lamp
	if (drawLamp)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		s_lamp->setMat4("MVP", proj * view * model);
		o_sphere->Draw(*s_lamp, DRAW_FLAGS_TRIANGLE_STRIP);
	}
}

void TestNormMap::OnImGuiRender()
{
	if (ImGui::Checkbox("Normal Mapping", &useNormMap))
		s_NormMap->setBool("useNormMap", useNormMap);
	if (ImGui::Checkbox("Visualise Normal Map", &visNormMap))
		s_NormMap->setBool("visNormMap", visNormMap);
	ImGui::Checkbox("Draw Lamp", &drawLamp);
}
