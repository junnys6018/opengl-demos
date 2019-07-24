#include "testPointShadow.h"
#include "debug.h"

TestPointShadows::TestPointShadows(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), visualiseDepthMap(false), lightPos(1.0f, 0.75f, 0.0f), lightColor(0.9f), time(0.0f)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);

	float cubeVertices[] = {
		// positions          // normals (inverted) // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f, 0.0f, 1.0f
	};
	m_CubeVB = std::make_unique<VertexBuffer>(cubeVertices, sizeof(cubeVertices));
	m_CubeVA = std::unique_ptr<VertexArray>(new VertexArray(*m_CubeVB, GL_FLOAT, { 3,3,2 }));
	t_Cube = std::make_unique<Texture>("res/Textures/wood.png");

	o_Sphere = std::make_unique<Object>("res/Objects/planet/planet.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Blastoise = std::make_unique<Object>("res/Objects/Pokemon/Blastoise/Blastoise.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Pikachu = std::make_unique<Object>("res/Objects/Pokemon/Pikachu/Pikachu.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Nidoqueen = std::make_unique<Object>("res/Objects/Pokemon/Nidoqueen/XY_Nidoqueen.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Piplup = std::make_unique<Object>("res/Objects/Pokemon/Piplup/Piplup.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	s_ShadowPass = std::make_unique<Shader>("res/shaders/Shadows/PointShadowPass.shader");
	s_RenderPass = std::make_unique<Shader>("res/shaders/Shadows/PointRenderPass.shader");
	s_RenderPass->setInt("depthMap", 1);
	s_RenderPass->setVec3("viewPos", m_camera.getCameraPos());
	s_RenderPass->setVec3("lightColor", lightColor);
	s_LampPass = std::make_unique<Shader>("res/shaders/Shadows/lamp5.shader");
	s_LampPass->setVec3("lightColor", lightColor);
	// Generate Model Matrices
	// cube
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(10.0f, 7.0f, 10.0f));
	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
	modelMatrices[0] = model;
	// Sphere
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f));
	modelMatrices[1] = model;
	// Blastoise
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
	model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
	modelMatrices[2] = model;
	// Pikachu
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrices[3] = model;
	// Nidoqueen
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.012f, 0.012f, 0.012f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrices[4] = model;
	// Piplup
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrices[5] = model;

	// Generate depthCubemap
	GLCall(glGenTextures(1, &depthCubeMap));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap));
	for (int i = 0; i < 6; i++)
	{
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	}
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	GLCall(glGenFramebuffers(1, &depthMapFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	// Generate LightSpace matrices
	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float near = 0.1f;
	float far = 10.0f;
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	s_ShadowPass->setFloat("far_plane", far);
	s_RenderPass->setFloat("far_plane", far);

	s_ShadowPass->setMat4("projMatrices[0]", proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	s_ShadowPass->setMat4("projMatrices[1]", proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	s_ShadowPass->setMat4("projMatrices[2]", proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	s_ShadowPass->setMat4("projMatrices[3]", proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	s_ShadowPass->setMat4("projMatrices[4]", proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	s_ShadowPass->setMat4("projMatrices[5]", proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	GLCall(glEnable(GL_DEPTH_TEST));
}

TestPointShadows::~TestPointShadows()
{
}

void TestPointShadows::OnUpdate()
{
	if (snapToLight)
		m_camera.setPos(lightPos);
	else if (m_camera.move(m_window))
		s_RenderPass->setVec3("viewPos", m_camera.getCameraPos());
	if (movePointLight)
	{
		// Update Light position
		time += m_camera.getDeltaT();
		if (time > 4.188790) // period is 4.188790s
			time = 0.0f;
		lightPos = glm::vec3(cosf(1.5f * time), 0.75, sinf(1.5f * time));
	}
	// Shadow Pass
	timer[0].begin();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
	GLCall(glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT));
	GLCall(glClear(GL_DEPTH_BUFFER_BIT));

	glm::mat4 lightTransform = glm::mat4(1.0f);
	lightTransform = glm::translate(lightTransform, -lightPos);
	s_ShadowPass->setMat4("lightTransform", lightTransform);

	configureModelsAndDraw(*s_ShadowPass, DRAW_FLAGS_NONE);
	timer[0].end();
	// Render Pass
	timer[1].begin();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, sWidth, sHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	s_RenderPass->setMat4("VP", proj * view);
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap));
	s_RenderPass->setInt("depthMap", 1);
	s_RenderPass->setVec3("lightPos", lightPos);

	configureModelsAndDraw(*s_RenderPass, DRAW_FLAGS_DIFFUSE);
	timer[1].end();
	// Lamp Pass
	timer[2].begin();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.1f));
	s_LampPass->setMat4("MVP", proj * view * model);
	m_CubeVA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	timer[2].end();
}
void TestPointShadows::configureModelsAndDraw(Shader& shader, Draw_Flags flags)
{
	m_CubeVA->Bind();
	t_Cube->Bind();
	shader.setMat4("model", modelMatrices[0]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	shader.setMat4("model", modelMatrices[1]);
	o_Sphere->Draw(shader, flags);

	shader.setMat4("model", modelMatrices[2]);
	o_Blastoise->Draw(shader, flags);

	shader.setMat4("model", modelMatrices[3]);
	o_Pikachu->Draw(shader, flags);

	shader.setMat4("model", modelMatrices[4]);
	o_Nidoqueen->Draw(shader, flags);

	shader.setMat4("model", modelMatrices[5]);
	o_Piplup->Draw(shader, flags);
}

void TestPointShadows::OnImGuiRender()
{
	ImGui::Checkbox("Snap to Light", &snapToLight);
	if (ImGui::Checkbox("Visualise Depth Map", &visualiseDepthMap))
		s_RenderPass->setBool("renderDepthMap", visualiseDepthMap);
	ImGui::Checkbox("Move Point Light", &movePointLight);
	if (ImGui::ColorEdit3("Light", &lightColor[0]))
	{
		s_RenderPass->setVec3("lightColor", lightColor);
		s_LampPass->setVec3("lightColor", lightColor);
	}

	ImGui::Separator();

	ImGui::Text("Shadow Pass: %.3f ms", (float)timer[0].getTime() / 1.0e6f);
	ImGui::Text("Lighting Pass: %.3f ms", (float)timer[1].getTime() / 1.0e6f);
	ImGui::Text("Lamp Pass: %.3f ms", (float)timer[2].getTime() / 1.0e6f);
	ImGui::Text("Total GPU Time: %.3f ms",
		(float)(timer[0].getTime() + timer[1].getTime() + timer[2].getTime()) / 1.0e6f);

	ImGui::Separator();

	ImGui::Text("x: %.2f y: %.2f z: %.2f", m_camera.getCameraPos().x, m_camera.getCameraPos().y, m_camera.getCameraPos().z);
}

void TestPointShadows::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
	}
}