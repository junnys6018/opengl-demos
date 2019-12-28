#include "testAdvLight.h"
#include "debug.h"

TestAdvLight::TestAdvLight(Base_Camera* cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_shadingMode(0), m_oldShadingMode(0), m_useGamma(1), m_oldUseGamma(1),
	m_shininess(4.0f)
	, m_renderLamps(true)
{
	// Floor
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
	};

	floorVB = std::make_unique<VertexBuffer>(planeVertices, sizeof(planeVertices));
	floorVA = std::unique_ptr<VertexArray>(new VertexArray(*floorVB, GL_FLOAT, { 3,3,2 }));
	t_floor = std::make_unique<Texture>("res/Textures/wood.png", GL_REPEAT, TEXTURE_INIT_FLAGS_GAMMA_CORRECT);
	t_floor->Bind(0);
	s_blinnPhong = std::make_unique<Shader>("res/Shaders/BlinnPhong.shader");
	s_blinnPhong->setInt("Texture1", 0);
	s_blinnPhong->setMat4("model", glm::mat4(1.0f));

	s_blinnPhong->setFloat("light.constant", 1.0f);
	s_blinnPhong->setFloat("light.lin", 0.09f);
	s_blinnPhong->setFloat("light.quadratic", 0.032f);

	s_blinnPhong->setInt("config.shadingMode", m_shadingMode);
	s_blinnPhong->setInt("config.useGamma", m_useGamma);
	s_blinnPhong->setFloat("config.shininess", m_shininess);

	// Lamp
	o_sphere = std::make_unique<Object>("res/Objects/sphere.obj");
	s_lamp = std::make_unique<Shader>("res/Shaders/lamp2.shader");
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	s_lamp->setMat4("model", model);
	// Uniform buffer
	u_matrix = std::unique_ptr<UniformBuffer>(new UniformBuffer({ MAT4,MAT4 }));
	u_matrix->Bind(0);
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	framebuffer_size_callback(sWidth, sHeight);

	GLCall(glEnable(GL_DEPTH_TEST));
}

TestAdvLight::~TestAdvLight()
{

}

void TestAdvLight::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	m_camera->handleWindowInput(m_window);

	glm::mat4 view = m_camera->getViewMatrix();
	u_matrix->setData(0, (void*)glm::value_ptr(view), MAT4);
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(m_camera->getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	u_matrix->setData(1, (void*)glm::value_ptr(proj), MAT4);

	s_blinnPhong->Use();
	s_blinnPhong->setVec3("light.position", glm::vec3(view * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	floorVA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	if (m_renderLamps)
		o_sphere->Draw(*s_lamp, DRAW_FLAGS_TRIANGLE_STRIP);
}

void TestAdvLight::OnImGuiRender()
{
	ImGui::RadioButton("Phong", &m_shadingMode, 0);
	ImGui::RadioButton("Blinn-Phong", &m_shadingMode, 1);
	ImGui::RadioButton("Both", &m_shadingMode, 2);
	if (m_shadingMode != m_oldShadingMode)
	{
		m_oldShadingMode = m_shadingMode;
		s_blinnPhong->setInt("config.shadingMode", m_shadingMode);
	}
	ImGui::Separator();
	ImGui::RadioButton("No Gamma", &m_useGamma, 0);
	ImGui::RadioButton("Gamma", &m_useGamma, 1);
	ImGui::RadioButton("Both##1", &m_useGamma, 2);
	if (m_useGamma != m_oldUseGamma)
	{
		m_oldUseGamma = m_useGamma;
		s_blinnPhong->setInt("config.useGamma", m_useGamma);
	}
	ImGui::Separator();
	if (ImGui::SliderFloat("Shininess", &m_shininess, 0.5f, 32.0f, "%.1f"))
	{
		s_blinnPhong->setFloat("config.shininess", m_shininess);
	}
	ImGui::Checkbox("Render Lamps", &m_renderLamps);

}

void TestAdvLight::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
		s_blinnPhong->setVec2("config.screenSize", width, height);
	}
}
