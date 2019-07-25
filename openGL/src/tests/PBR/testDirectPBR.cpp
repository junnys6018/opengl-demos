#include "testDirectPBR.h"
#include "debug.h"

TestDirectPBR::TestDirectPBR(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(0), oldrenderMode(0), metalness(0.0f), roughness(0.001f)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);

	o_Sphere = std::make_unique<Object>("res/Objects/sphere.obj");
	o_Cube = std::make_unique<Object>("res/Objects/cube.obj");
	float quadVertices[] = {
		// positions        // Normals       // texture Coords
		 2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
		-2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

		 2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		 2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f
	};
	QuadVB = std::make_unique<VertexBuffer>(quadVertices, sizeof(quadVertices));
	QuadVA = std::unique_ptr<VertexArray>(new VertexArray(*QuadVB, GL_FLOAT, { 3,3,2 }));

	s_Shader = std::make_unique<Shader>("res/shaders/PBR/Direct.shader");
	s_Lamp = std::make_unique<Shader>("res/shaders/Shadows/lamp5.shader");
	// initalise shader uniforms
	s_Shader->setVec3("viewPos", m_camera.getCameraPos());
	s_Shader->setFloat("roughness", roughness);
	for (int i = 0; i < 4; i++)
	{
		s_Shader->setVec3("lightPos[" + std::to_string(i) + "]", lightPos[i]);
		s_Shader->setVec3("lightColor[" + std::to_string(i) + "]", lightColor[i]);
	}
	s_Shader->setInt("Albedo", 0);
	s_Shader->setInt("Normal", 1);
	s_Shader->setInt("Metalness", 2);
	s_Shader->setInt("Roughness", 3);

	t_Albedo = std::make_unique<Texture>("res/Textures/PBR/future_panel/d.jpg");
	t_Normal = std::make_unique<Texture>("res/Textures/PBR/future_panel/n.jpg");
	t_Metalness = std::make_unique<Texture>("res/Textures/PBR/future_panel/m.jpg");
	t_Roughness = std::make_unique<Texture>("res/Textures/PBR/future_panel/r.jpg");

	t_Albedo->Bind(0);
	t_Normal->Bind(1);
	t_Metalness->Bind(2);
	t_Roughness->Bind(3);

	GLCall(glEnable(GL_DEPTH_TEST));
}

TestDirectPBR::~TestDirectPBR()
{
}

void TestDirectPBR::OnUpdate()
{
	timer.begin();
	if (m_camera.move(m_window))
	{
		s_Shader->setVec3("viewPos", m_camera.getCameraPos());
	}
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	s_Shader->setMat4("VP", proj * view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f));
	s_Shader->setMat4("model", model);
	s_Shader->setBool("useSamplers", false);
	o_Sphere->Draw(*s_Shader, DRAW_FLAGS_TRIANGLE_STRIP);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(1.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Shader->setMat4("model", model);
	s_Shader->setBool("useSamplers", true);
	QuadVA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	// o_Sphere->Draw(*s_Shader, DRAW_FLAGS_TRIANGLE_STRIP);

	for (int i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos[i]);
		model = glm::scale(model, glm::vec3(0.3f));

		glm::vec3 color = lightColor[i] / (lightColor[i] + glm::vec3(1.0));
		color = glm::pow(color, glm::vec3(1.0 / 2.2));

		s_Lamp->setMat4("MVP", proj * view * model);
		s_Lamp->setVec3("lightColor", color);

		o_Cube->Draw(*s_Lamp);
	}
	timer.end();
}

void TestDirectPBR::OnImGuiRender()
{
	ImGui::Text("%.3f ms / frame", timer.getTime() / 1.0e6f);
	if (ImGui::SliderFloat("Metalness", &metalness, 0.0f, 1.0f))
	{
		s_Shader->setFloat("metalness", metalness);
	}
	if (ImGui::SliderFloat("Roughness", &roughness, 0.001f, 1.0f))
	{
		s_Shader->setFloat("roughness", roughness);
	}
	ImGui::RadioButton("Lighting", &renderMode, 0);
	ImGui::RadioButton("Diffuse", &renderMode, 1);
	ImGui::RadioButton("Specular", &renderMode, 2);
	ImGui::RadioButton("Distribution", &renderMode, 3);
	ImGui::RadioButton("Freshnel", &renderMode, 4);
	ImGui::RadioButton("Geometry", &renderMode, 5);
	// only update flags if it changed
	if (renderMode != oldrenderMode)
	{
		oldrenderMode = renderMode;
		s_Shader->setInt("renderMode", renderMode);
	}
}

void TestDirectPBR::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
	}
}
