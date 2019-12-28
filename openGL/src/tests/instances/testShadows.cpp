#include "testShadows.h"
#include "debug.h"

TestShadows::TestShadows(Base_Camera* cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderDepthMap(false), m_lightAngle(45.0f)
{
	// Object Loading
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);

	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,


		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f
	};

	m_floorVB = std::make_unique<VertexBuffer>(planeVertices, sizeof(planeVertices));
	m_floorVA = std::unique_ptr<VertexArray>(new VertexArray(*m_floorVB, GL_FLOAT, { 3,3,2 }));
	t_floor = std::make_unique<Texture>("res/Textures/wood.png");

	o_sphere = std::make_unique<Object>("res/Objects/planet/planet.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	float cubeVertices[] = {
		// positions          // normals           // texture coords
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left    
	};
	m_cubeVB = std::make_unique<VertexBuffer>(cubeVertices, sizeof(cubeVertices));
	m_cubeVA = std::unique_ptr<VertexArray>(new VertexArray(*m_cubeVB, GL_FLOAT, { 3,3,2 }));
	t_cube = std::make_unique<Texture>("res/Textures/container.jpg");

	float quadVertices[] = {
		// positions   // texCoords
		 0.5f,  1.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.0f, 0.0f,
		 1.0f,  0.5f,  1.0f, 0.0f,

		 0.5f,  1.0f,  0.0f, 1.0f,
		 1.0f,  0.5f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	m_quadVB = std::make_unique<VertexBuffer>(quadVertices, sizeof(quadVertices));
	m_quadVA = std::unique_ptr<VertexArray>(new VertexArray(*m_quadVB, GL_FLOAT, { 2,2 }));

	s_renderPass = std::make_unique<Shader>("res/Shaders/Shadows/RenderPass.shader");
	s_renderPass->setInt("Texture1", 0);
	s_renderPass->setInt("depthMap", 1);
	s_shadowPass = std::make_unique<Shader>("res/Shaders/Shadows/ShadowPass.shader");
	s_depthMap = std::make_unique<Shader>("res/Shaders/Shadows/DepthMap.shader");
	s_depthMap->setInt("depthMap", 1);

	// FrameBuffer init
	GLCall(glGenFramebuffers(1, &depthMapFBO));

	GLCall(glGenTextures(1, &depthMap));
	GLCall(glBindTexture(GL_TEXTURE_2D, depthMap));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT
		, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
	
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	// uniform buffer
	u_matrix = std::unique_ptr<UniformBuffer>(new UniformBuffer({ MAT4 }));
	glm::mat4 model = glm::mat4(1.0f);
	modelMats.push_back(model);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, 0.5f, 2.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	modelMats.push_back(model);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-0.5f, 1.5f, -0.5f));
	model = glm::scale(model, glm::vec3(0.5f));
	modelMats.push_back(model);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.5f));
	model = glm::scale(model, glm::vec3(0.5f));
	modelMats.push_back(model);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 1.5));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	modelMats.push_back(model);
	// lightspace matrix
	calcLightFromAngle();

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_CULL_FACE));
}

TestShadows::~TestShadows()
{
	GLCall(glDisable(GL_CULL_FACE));
	GLCall(glDeleteFramebuffers(1, &depthMapFBO));
	GLCall(glDeleteTextures(1, &depthMap));
}

void TestShadows::OnUpdate()
{
	m_camera->handleWindowInput(m_window);

	timer[0].begin();
	shadowPass();
	timer[0].end();

	timer[1].begin();
	renderPass();
	timer[1].end();

	timer[2].begin();
	if (renderDepthMap)
	{
		m_quadVA->Bind();
		s_depthMap->Use();
		GLCall(glActiveTexture(GL_TEXTURE1));
		GLCall(glBindTexture(GL_TEXTURE_2D, depthMap));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	}
	timer[2].end();
}

void TestShadows::OnImGuiRender()
{
	ImGui::Checkbox("Render depth map", &renderDepthMap);
	if (ImGui::SliderFloat("angle", &m_lightAngle, 10.0f, 170.0f, "%.0f"))
		calcLightFromAngle();
	ImGui::Text("Shadow Pass: %.3f ms", (float)timer[0].getTime() / 1.0e6f);
	ImGui::Text("Render (Lighting) Pass: %.3f ms", (float)timer[1].getTime() / 1.0e6f);
	ImGui::Text("Rendering Depth: %.3f ms", (float)timer[2].getTime() / 1.0e6f);
	ImGui::Text("Total GPU Time: %.3f ms",
		(float)(timer[0].getTime() + timer[1].getTime() + timer[2].getTime()) / 1.0e6f);
}

void TestShadows::shadowPass()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
	GLCall(glClear(GL_DEPTH_BUFFER_BIT));
	GLCall(glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT));

	s_shadowPass->Use();
	u_matrix->Bind(0);
	// Floor
	s_shadowPass->setMat4("model", modelMats[0]);
	m_floorVA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	GLCall(glCullFace(GL_FRONT));
	// Sphere
	s_shadowPass->setMat4("model", modelMats[1]);
	o_sphere->vertexArray->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, o_sphere->indexBuffer->getCount(), GL_UNSIGNED_INT, 0));

	// Cube
	m_cubeVA->Bind();
	s_shadowPass->setMat4("model", modelMats[2]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	// Second cube is touching floor, dont facecull
	GLCall(glCullFace(GL_BACK));
	s_shadowPass->setMat4("model", modelMats[3]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	GLCall(glCullFace(GL_FRONT));

	s_shadowPass->setMat4("model", modelMats[4]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	GLCall(glCullFace(GL_BACK));
}

void TestShadows::renderPass()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCall(glViewport(0, 0, sWidth, sHeight));

	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, depthMap));

	glm::mat4 view = m_camera->getViewMatrix();
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(m_camera->getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	s_renderPass->setMat4("VP", proj * view);
	s_renderPass->setVec3("viewPos", m_camera->getCameraPos());
	u_matrix->Bind(0);
	// Floor
	s_renderPass->setMat4("model", modelMats[0]);
	m_floorVA->Bind();
	t_floor->Bind(0);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	// Sphere
	s_renderPass->setMat4("model", modelMats[1]);
	o_sphere->Draw(*s_renderPass);

	// Cube
	m_cubeVA->Bind();
	t_cube->Bind(0);
	s_renderPass->setMat4("model", modelMats[2]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	s_renderPass->setMat4("model", modelMats[3]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	s_renderPass->setMat4("model", modelMats[4]);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
}
void TestShadows::calcLightFromAngle()
{
	float near_plane = 0.0f, far_plane = 9.0f;
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);

	float lightAngleRad = glm::radians(m_lightAngle);
	glm::vec3 lightPos = glm::vec3(cosf(lightAngleRad), 4.5f * sinf(lightAngleRad), 2 * cosf(lightAngleRad));
	float up = m_lightAngle < 90.0f ? 1.0f : -1.0f;
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, up, 0.0f));
	s_renderPass->setVec3("lightDir", glm::normalize(-lightPos));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	u_matrix->setData(0, glm::value_ptr(lightSpaceMatrix), MAT4);
}
void TestShadows::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
	}
}

