#include "testParallaxMap.h"
#include "debug.h"

TestParaMap::TestParaMap(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(0), oldrenderMode(0)
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

	t_Brick_DiffMap = std::make_unique<Texture>("res/Textures/bricks2.jpg");
	t_Brick_NormMap = std::make_unique<Texture>("res/Textures/bricks2_normal.jpg");
	t_Brick_DispMap = std::make_unique<Texture>("res/Textures/bricks2_disp.jpg");

	t_Wood_DiffMap = std::make_unique<Texture>("res/Textures/toy_box_diffuse.png");
	t_Wood_NormMap = std::make_unique<Texture>("res/Textures/toy_box_normal.png");
	t_Wood_DispMap = std::make_unique<Texture>("res/Textures/toy_box_disp.png");

	s_ParaMap = std::make_unique<Shader>("res/Shaders/ParaMap.shader");
	s_ParaMap->setVec3("v_lightPos", 0.0f, 0.0f, 1.5f);
	s_ParaMap->setVec3("v_viewPos", m_camera.getCameraPos());

	s_ParaMap->setInt("Texture1", 0);
	s_ParaMap->setInt("NormMap", 1);
	s_ParaMap->setInt("DispMap", 2);

	GLCall(glEnable(GL_DEPTH_TEST));
}

TestParaMap::~TestParaMap()
{
}

void TestParaMap::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	if (m_camera.move(m_window))
		s_ParaMap->setVec3("v_viewPos", m_camera.getCameraPos());
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(width) / height, 0.1f, 100.0f);
	s_ParaMap->setMat4("model", model);
	s_ParaMap->setMat4("view", view);
	s_ParaMap->setMat4("proj", proj);

	s_ParaMap->Use();
	quadVA->Bind();
	// Brick Wall
	t_Wood_DiffMap->Bind(0);
	t_Brick_NormMap->Bind(1);
	t_Brick_DispMap->Bind(2);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	// Wood Wall
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_ParaMap->setMat4("model", model);
	t_Brick_DiffMap->Bind(0);
	t_Wood_NormMap->Bind(1);
	t_Wood_DispMap->Bind(2);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void TestParaMap::OnImGuiRender()
{
	ImGui::RadioButton("Nothing", &renderMode, 0);
	ImGui::RadioButton("Normal Mapping", &renderMode, 1);
	ImGui::RadioButton("Displacement Mapping", &renderMode, 2);
	if (renderMode != oldrenderMode)
	{
		oldrenderMode = renderMode;
		s_ParaMap->setInt("renderMode", renderMode);
	}
}
