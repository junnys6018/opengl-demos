#include "testDirectPBR.h"
#include "debug.h"

TestDirectPBR::TestDirectPBR(Base_Camera* cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(0), oldrenderMode(0), metalness(0.0f), roughness(0.001f), paraMapping(false)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);

	o_Sphere = std::make_unique<Object>("res/Objects/sphere.obj");
	o_Cylinder = std::make_unique<Object>("res/Objects/cylinder.obj", OBJECT_INIT_FLAGS_GEN_TANGENT);
	o_Cube = std::make_unique<Object>("res/Objects/cube.obj");

	s_Shader = std::make_unique<Shader>("res/shaders/PBR/Direct.shader");
	s_Lamp = std::make_unique<Shader>("res/shaders/Shadows/lamp5.shader");
	// initalise shader uniforms
	s_Shader->setVec3("viewPos", m_camera->getCameraPos());
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
	s_Shader->setInt("DispMap", 4);

	generateTextures("ivory_panel", ".jpg");

	GLCall(glEnable(GL_DEPTH_TEST));
}

TestDirectPBR::~TestDirectPBR()
{
}

void TestDirectPBR::OnUpdate()
{
	timer.begin();
	if (m_camera->handleWindowInput(m_window))
	{
		s_Shader->setVec3("viewPos", m_camera->getCameraPos());
	}
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 view = m_camera->getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera->getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	s_Shader->setMat4("VP", proj * view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f));
	s_Shader->setMat4("model", model);
	s_Shader->setBool("useSamplers", false);
	o_Sphere->Draw(*s_Shader, DRAW_FLAGS_TRIANGLE_STRIP);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 0.05f * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(3.0f));
	s_Shader->setMat4("model", model);
	s_Shader->setBool("useSamplers", true);
	o_Cylinder->Draw(*s_Shader);

	for (int i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos[i]);
		model = glm::scale(model, glm::vec3(0.3f));

		glm::vec3 color = lightColor[i] / (lightColor[i] + glm::vec3(1.0));
		color = glm::pow(color, glm::vec3(1.0f / 2.2f));

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
	if (ImGui::RadioButton("Normal Mapping", (int*)& paraMapping, 0) || ImGui::RadioButton("Para Mapping", (int*)& paraMapping, 1))
	{
		s_Shader->setBool("paraMapping", paraMapping);
	}
	if (ImGui::CollapsingHeader("Render Target"))
	{
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
	if (ImGui::CollapsingHeader("Textures"))
	{
		if (ImGui::Button("Future Panel", ImVec2(120.0f, 25.0f)))
			generateTextures("future_panel", ".jpg");
		if (ImGui::Button("Ivory Panel", ImVec2(120.0f, 25.0f)))
			generateTextures("ivory_panel", ".jpg");
		if (ImGui::Button("Bath Tile", ImVec2(120.0f, 25.0f)))
			generateTextures("bath_tile", ".jpg");
		if (ImGui::Button("Mosaic", ImVec2(120.0f, 25.0f)))
			generateTextures("mosaic", ".jpg");
		if (ImGui::Button("Subway Tile", ImVec2(120.0f, 25.0f)))
			generateTextures("subway_tile", ".jpg");
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

void TestDirectPBR::generateTextures(std::string name, std::string format)
{
	t_Albedo.get_deleter();
	t_Normal.get_deleter();
	t_Metalness.get_deleter();
	t_Roughness.get_deleter();
	t_Displacement.get_deleter();

	t_Albedo = std::make_unique<Texture>("res/Textures/PBR/" + name + "/d" + format);
	t_Normal = std::make_unique<Texture>("res/Textures/PBR/" + name + "/n" + format);
	t_Metalness = std::make_unique<Texture>("res/Textures/PBR/" + name + "/m" + format);
	t_Roughness = std::make_unique<Texture>("res/Textures/PBR/" + name + "/r" + format);
	t_Displacement = std::make_unique<Texture>("res/Textures/PBR/" + name + "/h" + format);

	t_Albedo->Bind(0);
	t_Normal->Bind(1);
	t_Metalness->Bind(2);
	t_Roughness->Bind(3);
	t_Displacement->Bind(4);
}
