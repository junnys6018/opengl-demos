#include "testSSAO.h"
#include "debug.h"
#include <cstdlib> // for rand() and srand()
float rand_float(float a, float b);

TestSSAO::TestSSAO(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(5), oldRenderMode(5), LightColor(1.0f, 1.0f, 1.0f)
{
	srand((unsigned int)glfwGetTime());
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	o_NanoSuit = std::make_unique<Object>("res/Objects/nanosuit/nanosuit.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Erato = std::make_unique<Object>("res/Objects/erato/erato-1.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	s_GeometryPass = std::make_unique<Shader>("res/shaders/SSAO/GeometryPass.shader");
	s_ssaoPass = std::make_unique<Shader>("res/shaders/SSAO/OcclusionPass.shader");
	s_ssaoPass->setInt("gPosition", 0);
	s_ssaoPass->setInt("gNormal", 1);
	s_ssaoPass->setInt("texNoise", 2);
	s_ssaoPass->setVec2("screenSize", (float)sWidth, (float)sHeight);
	s_BlurPass = std::make_unique<Shader>("res/shaders/SSAO/BlurPass.shader");
	s_BlurPass->setInt("ssaoColor", 0);
	s_LightingPass = std::make_unique<Shader>("res/shaders/SSAO/LightingPass.shader");
	s_LightingPass->setInt("gPosition", 0);
	s_LightingPass->setInt("gNormal", 1);
	s_LightingPass->setInt("gAlbedo", 2);
	s_LightingPass->setInt("SSAO", 3);
	s_LightingPass->setInt("renderMode", 5);
	s_LightingPass->setVec3("light.Color", LightColor);

	// Screen quad VAO
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	QuadVB = std::make_unique<VertexBuffer>(quadVertices, sizeof(quadVertices));
	QuadVA = std::unique_ptr<VertexArray>(new VertexArray(*QuadVB, GL_FLOAT, { 2,2 }));
	// cube setup
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
	CubeVB = std::make_unique<VertexBuffer>(cubeVertices, sizeof(cubeVertices));
	CubeVA = std::unique_ptr<VertexArray>(new VertexArray(*CubeVB, GL_FLOAT, { 3,3,2 }));
	CubeTexture = std::make_unique<Texture>("res/Textures/container.jpg");

	genFrameBuffers();

	// Generate Hemisphere Kernel
	for (int i = 0; i < 64; i++)
	{
		glm::vec3 sample(
			rand_float(-1.0f, 1.0f),
			rand_float(-1.0f, 1.0f),
			rand_float(0.0f, 1.0f)
		);
		sample = glm::normalize(sample);
		float scale = (float)i / 64.0f;
		scale = 0.1f + scale * scale * 0.9f;
		sample *= scale;
		s_ssaoPass->setVec3("samples[" + std::to_string(i) + "]", sample);
	}

	// Generate random rotation vectors
	std::vector<glm::vec3> ssaoNoise;
	for (int i = 0; i < 16; i++)
	{
		ssaoNoise.emplace_back(rand_float(-1.0f, 1.0f), rand_float(-1.0f, 1.0f), 0.0f);
	}
	// we send rotation vectors via texture
	GLCall(glGenTextures(1, &noiseTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, noiseTexture));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

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

	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	// Geometry Pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, gBuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));
	model = glm::translate(model, glm::vec3(11440.0f, 13000.0f, 12730.0f));
	s_GeometryPass->setMat4("model", model);
	s_GeometryPass->setMat4("view", view);
	s_GeometryPass->setMat4("proj", proj);
	o_Erato->Draw(*s_GeometryPass);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f));
	s_GeometryPass->setMat4("model", model);
	o_NanoSuit->Draw(*s_GeometryPass);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
	model = glm::scale(model, glm::vec3(8.0f));
	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
	s_GeometryPass->setMat4("model", model);
	CubeVA->Bind();
	CubeTexture->Bind(0);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	// SSAO Pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, gPosition));

	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, gNormal));

	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, noiseTexture));
	s_ssaoPass->setMat4("proj", proj);
	QuadVA->Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	//// Blur Pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, ssaoColor));
	s_BlurPass->Use();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	// final pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, gPosition));

	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, gNormal));

	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, gColor));

	GLCall(glActiveTexture(GL_TEXTURE3));
	GLCall(glBindTexture(GL_TEXTURE_2D, (renderMode == 3 ? ssaoColor : ssaoBlurColor)));
	s_LightingPass->setVec3("light.Position", glm::vec3(view * glm::vec4(-1.7f, 2.3f, 2.0f, 1.0f)));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void TestSSAO::OnImGuiRender()
{
	ImGui::RadioButton("gPosition"     , &renderMode, 0);
	ImGui::RadioButton("gNormal"       , &renderMode, 1);
	ImGui::RadioButton("gAlbedo"       , &renderMode, 2);
	ImGui::RadioButton("SSAO Texture"  , &renderMode, 3);
	ImGui::RadioButton("SSAO Blur"     , &renderMode, 4);
	ImGui::RadioButton("SSAO Lighting" , &renderMode, 5);
	ImGui::RadioButton("Lighting"      , &renderMode, 6);
	// only rendermode if it changed
	if (renderMode != oldRenderMode)
	{
		oldRenderMode = renderMode;
		s_LightingPass->setInt("renderMode", renderMode);
	}
	ImGui::Separator();
	if (ImGui::ColorEdit3("Light Color", &LightColor[0]))
		s_LightingPass->setVec3("light.Color", LightColor);

}

void TestSSAO::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
		genFrameBuffers();
		s_ssaoPass->setVec2("screenSize", (float)sWidth, (float)sHeight);
	}
}

void TestSSAO::genFrameBuffers()
{
	GLCall(glDeleteFramebuffers(1, &gBuffer));
	GLCall(glDeleteTextures(1, &gPosition));
	GLCall(glDeleteTextures(1, &gNormal));
	GLCall(glDeleteTextures(1, &gColor));
	GLCall(glDeleteRenderbuffers(1, &rboDepth));

	GLCall(glGenFramebuffers(1, &gBuffer));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, gBuffer));

	// Position Buffer
	GLCall(glGenTextures(1, &gPosition));
	GLCall(glBindTexture(GL_TEXTURE_2D, gPosition));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, sWidth, sHeight, 0, GL_RGB, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// Normal Buffer
	GLCall(glGenTextures(1, &gNormal));
	GLCall(glBindTexture(GL_TEXTURE_2D, gNormal));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, sWidth, sHeight, 0, GL_RGB, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0));

	// Color Buffer
	GLCall(glGenTextures(1, &gColor));
	GLCall(glBindTexture(GL_TEXTURE_2D, gColor));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sWidth, sHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColor, 0));

	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	GLCall(glDrawBuffers(3, attachments));

	// create and attach depth buffer (renderbuffer)
	GLCall(glGenRenderbuffers(1, &rboDepth));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboDepth));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sWidth, sHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth));


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	// SSAO
	GLCall(glDeleteFramebuffers(1, &ssaoFBO));
	GLCall(glGenFramebuffers(1, &ssaoFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO));

	GLCall(glGenTextures(1, &ssaoColor));
	GLCall(glBindTexture(GL_TEXTURE_2D, ssaoColor));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sWidth, sHeight, 0, GL_RGB, GL_FLOAT, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColor, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;

	GLCall(glDeleteFramebuffers(1, &ssaoBlurFBO));
	GLCall(glGenFramebuffers(1, &ssaoBlurFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO));

	GLCall(glGenTextures(1, &ssaoBlurColor));
	GLCall(glBindTexture(GL_TEXTURE_2D, ssaoBlurColor));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sWidth, sHeight, 0, GL_RGB, GL_FLOAT, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColor, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
}

float rand_float(float a, float b) // uniform random float in [a,b]
{
	assert(b > a);
	return (b - a) * (float)rand() / (float)RAND_MAX + a;
}
