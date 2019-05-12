#include "testDeferred.h"
#include "debug.h"

TestDeferred::TestDeferred(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(0), old_renderMode(0)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	genFrameBuffers();

	s_GeometryPass = std::make_unique<Shader>("res/shaders/DeferredShading/GeometryPass.shader");
	s_LightPass = std::make_unique<Shader>("res/shaders/DeferredShading/LightPass.shader");
	s_LightPass->setInt("gPosition", 0);
	s_LightPass->setInt("gNormal", 1);
	s_LightPass->setInt("gAlbedoSpec", 2);


	o_Sponza = std::make_unique<Object>("res/Objects/sponza/sponza.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE | OBJECT_INIT_FLAGS_GEN_SPECULAR);

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

	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glEnable(GL_DEPTH_TEST));
}

TestDeferred::~TestDeferred()
{
	GLCall(glDeleteFramebuffers(1, &gBuffer));
	GLCall(glDeleteTextures(1, &gPosition));
	GLCall(glDeleteTextures(1, &gNormal));
	GLCall(glDeleteTextures(1, &gColorSpec));
	GLCall(glDeleteRenderbuffers(1, &rboDepth));
}

void TestDeferred::OnUpdate()
{
	m_camera.move(m_window);
	// Geometry Pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, gBuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);

	s_GeometryPass->setMat4("model", model);
	s_GeometryPass->setMat4("view", view);
	s_GeometryPass->setMat4("proj", proj);

	o_Sponza->Draw(*s_GeometryPass, DRAW_FLAGS_DIFFUSE | DRAW_FLAGS_SPECULAR);

	// Lighting Pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, gPosition));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, gNormal));
	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, gColorSpec));

	QuadVA->Bind();
	s_LightPass->Use();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

}

void TestDeferred::OnImGuiRender()
{
	ImGui::RadioButton("Show Lighting", &renderMode, 0);
	ImGui::RadioButton("Show Position", &renderMode, 1);
	ImGui::RadioButton("Show Normal", &renderMode, 2);
	ImGui::RadioButton("Show Albedo", &renderMode, 3);
	ImGui::RadioButton("Show Specular", &renderMode, 4);

	if (renderMode != old_renderMode)
	{
		old_renderMode = renderMode;
		s_LightPass->setInt("renderMode", renderMode);
	}
}

void TestDeferred::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
		genFrameBuffers();
	}
}

void TestDeferred::genFrameBuffers()
{
	GLCall(glDeleteFramebuffers(1, &gBuffer));
	GLCall(glDeleteTextures(1, &gPosition));
	GLCall(glDeleteTextures(1, &gNormal));
	GLCall(glDeleteTextures(1, &gColorSpec));
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

	// Normal Buffer
	GLCall(glGenTextures(1, &gNormal));
	GLCall(glBindTexture(GL_TEXTURE_2D, gNormal));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, sWidth, sHeight, 0, GL_RGB, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0));

	// Color and Specular Buffer
	GLCall(glGenTextures(1, &gColorSpec));
	GLCall(glBindTexture(GL_TEXTURE_2D, gColorSpec));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sWidth, sHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0));

	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	GLCall(glDrawBuffers(3, attachments));

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sWidth, sHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
