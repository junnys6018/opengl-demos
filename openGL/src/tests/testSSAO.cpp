#include "testSSAO.h"
#include "debug.h"
#include <cstdlib> // for rand() and srand()
float rand_float(float a, float b);

TestSSAO::TestSSAO(Base_Camera* cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(5), oldRenderMode(5), LightColor(1.0f, 1.0f, 1.0f), power(1.0f)
{
	srand((unsigned int)glfwGetTime());
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	o_NanoSuit = std::make_unique<Object>("res/Objects/nanosuit/nanosuit.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Pikachu = std::make_unique<Object>("res/Objects/Pokemon/Pikachu/Pikachu.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	s_GeometryPass = std::make_unique<Shader>("res/shaders/SSAO/GeometryPass.shader");
	s_ssaoPass = std::make_unique<Shader>("res/shaders/SSAO/OcclusionPass.shader");
	s_ssaoPass->setInt("gPosition", 0);
	s_ssaoPass->setInt("gNormal", 1);
	s_ssaoPass->setInt("texNoise", 2);
	s_ssaoPass->setVec2("screenSize", (float)sWidth, (float)sHeight);
	s_ssaoPass->setFloat("power", power);
	s_BlurPass = std::make_unique<Shader>("res/shaders/SSAO/BlurPass.shader");
	s_BlurPass->setInt("ssaoColor", 0);
	s_LightingPass = std::make_unique<Shader>("res/shaders/SSAO/LightingPass.shader");
	s_LightingPass->setInt("gPosition", 0);
	s_LightingPass->setInt("gNormal", 1);
	s_LightingPass->setInt("gAlbedo", 2);
	s_LightingPass->setInt("SSAO", 3);
	s_LightingPass->setInt("renderMode", 5);
	s_LightingPass->setVec3("light.Color", LightColor);
	s_LampPass = std::make_unique<Shader>("res/shaders/Shadows/lamp5.shader");

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
	CubeTexture = std::make_unique<Texture>("res/Textures/wood.png");

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
	GLCall(glDeleteFramebuffers(1, &gBuffer));
	GLCall(glDeleteTextures(1, &gPosition));
	GLCall(glDeleteTextures(1, &gNormal));
	GLCall(glDeleteTextures(1, &gColor));
	GLCall(glDeleteRenderbuffers(1, &rboDepth));

	GLCall(glDeleteFramebuffers(1, &ssaoFBO));
	GLCall(glDeleteTextures(1, &ssaoColor));

	GLCall(glDeleteFramebuffers(1, &ssaoBlurFBO));
	GLCall(glDeleteTextures(1, &ssaoBlurColor));
}

void TestSSAO::OnUpdate()
{
	m_camera->handleWindowInput(m_window);
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 view = m_camera->getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera->getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	LightViewPos = glm::vec3(view * glm::vec4(-1.7f, 0.3f, 0.0f, 1.0f));
	// Geometry Pass
	timer[0].begin();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, gBuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, -2.0f));
	model = glm::scale(model, glm::vec3(0.4f));
	s_GeometryPass->setMat4("model", model);
	s_GeometryPass->setMat4("view", view);
	s_GeometryPass->setMat4("proj", proj);
	o_Pikachu->Draw(*s_GeometryPass);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 1.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f));
	s_GeometryPass->setMat4("model", model);
	o_NanoSuit->Draw(*s_GeometryPass);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(8.0f));
	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
	s_GeometryPass->setMat4("model", model);
	CubeVA->Bind();
	CubeTexture->Bind(0);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	timer[0].end();
	// SSAO Pass
	timer[1].begin();
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
	timer[1].end();
	// Blur Pass
	timer[2].begin();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, ssaoColor));
	s_BlurPass->Use();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	timer[2].end();
	// Lighting Pass
	timer[3].begin();
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
	s_LightingPass->setVec3("light.Position", LightViewPos);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	timer[3].end();
	// Lamp Pass
	timer[4].begin();

	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0)); // write to default framebuffer
	GLCall(glBlitFramebuffer(0, 0, sWidth, sHeight, 0, 0, sWidth, sHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST));

	CubeVA->Bind();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.7f, 0.3f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f));
	s_LampPass->setMat4("MVP", proj * view * model);
	s_LampPass->setVec3("lightColor", LightColor);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	timer[4].end();
}

void TestSSAO::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Render Targets"))
	{
		ImGui::RadioButton("gPosition"     , &renderMode, 0);
		ImGui::RadioButton("gNormal"       , &renderMode, 1);
		ImGui::RadioButton("gAlbedo"       , &renderMode, 2);
		ImGui::RadioButton("SSAO Texture"  , &renderMode, 3);
		ImGui::RadioButton("SSAO Blur"     , &renderMode, 4);
		ImGui::RadioButton("SSAO Lighting" , &renderMode, 5);
		ImGui::RadioButton("Lighting"      , &renderMode, 6);
		// only update rendermode if it changed
		if (renderMode != oldRenderMode)
		{
			oldRenderMode = renderMode;
			s_LightingPass->setInt("renderMode", renderMode);
		}
	}
	if (ImGui::CollapsingHeader("Profiling"))
	{
		ImGui::Text("Geometry Pass: %.3f ms"  , (float)timer[0].getTime() / 1.0e6f);
		ImGui::Text("SSAO Pass: %.3f ms"      , (float)timer[1].getTime() / 1.0e6f);
		ImGui::Text("Blur Pass: %.3f ms"      , (float)timer[2].getTime() / 1.0e6f);
		ImGui::Text("Lighting Pass: %.3f ms"  , (float)timer[3].getTime() / 1.0e6f);
		ImGui::Text("Lamp Pass: %.3f ms"      , (float)timer[4].getTime() / 1.0e6f);
		ImGui::Text("Total GPU Time: %.3f ms" ,
			(float)(timer[0].getTime() + timer[1].getTime() + timer[2].getTime() + timer[3].getTime() + timer[4].getTime()) / 1.0e6f);
	}
	if (ImGui::ColorEdit3("Light Color", &LightColor[0]))
		s_LightingPass->setVec3("light.Color", LightColor);
	if (ImGui::SliderFloat("Occlusion pow", &power, 1.0f, 16.0f))
		s_ssaoPass->setFloat("power", power);

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
	GLCall(glDeleteTextures(1, &ssaoColor));
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
	GLCall(glDeleteTextures(1, &ssaoBlurColor));
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
