#include "testBloom.h"
#include "debug.h"
#define BLUR_SCALE 10
TestBloom::TestBloom(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_isWireFrame(false), exposure(0.1f), renderMode(0), old_renderMode(0)
{
	s_Bloom = std::make_unique<Shader>("res/Shaders/Bloom/Bloom.shader");
	s_Bloom->setVec3("viewPos", m_camera.getCameraPos());
	s_Lamp = std::make_unique<Shader>("res/Shaders/Bloom/lamp3.shader");
	s_Blur = std::make_unique<Shader>("res/Shaders/Bloom/Blur.shader");
	s_Blur->setInt("image", 0);
	s_Final = std::make_unique<Shader>("res/Shaders/Bloom/Final.shader");
	s_Final->setInt("scene", 0);
	s_Final->setInt("blur", 1);
	s_Final->setFloat("exposure", exposure);

	o_Sponza = std::make_unique<Object>("res/Objects/sponza/sponza.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Cube = std::make_unique<Object>("res/Objects/cube.obj");
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

	u_Matrices = std::unique_ptr<UniformBuffer>(new UniformBuffer({ MAT4,MAT4 }));
	u_Matrices->Bind(0);

	u_Lights = std::unique_ptr<UniformBuffer>(new UniformBuffer({ VEC3, VEC3, VEC3, VEC3, VEC3, VEC3, VEC3, VEC3 }));
	u_Lights->setData(0, (void*)glm::value_ptr(glm::vec3(-10.0f, 2.5f, -0.35f)), VEC3);
	u_Lights->setData(1, (void*)glm::value_ptr(glm::vec3(9.0f, 2.5f, -0.35f))  , VEC3);
	u_Lights->setData(2, (void*)glm::value_ptr(glm::vec3(2.5f, 2.0f, -0.35f))  , VEC3);
	u_Lights->setData(3, (void*)glm::value_ptr(glm::vec3(11.2f, 2.5f, 4.0f)) , VEC3);

	u_Lights->setData(4, (void*)glm::value_ptr(glm::vec3(15.0f, 10.0f, 0.0f)), VEC3);
	u_Lights->setData(5, (void*)glm::value_ptr(glm::vec3(10.0f, 0.0f, 10.0f)), VEC3);
	u_Lights->setData(6, (void*)glm::value_ptr(glm::vec3(0.0f, 10.0f, 10.0f)), VEC3);
	u_Lights->setData(7, (void*)glm::value_ptr(glm::vec3(0.0f, 5.0f, 0.0f)), VEC3);

	u_Lights->Bind(1);

	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	framebuffer_size_callback(sWidth, sHeight);

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_CULL_FACE));
}

TestBloom::~TestBloom()
{
	GLCall(glDisable(GL_CULL_FACE));
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

void TestBloom::OnUpdate()
{
	if (m_camera.move(m_window))
		s_Bloom->setVec3("viewPos", m_camera.getCameraPos());

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	GLCall(glPolygonMode(GL_FRONT_AND_BACK, m_isWireFrame ? GL_LINE : GL_FILL));
	// Sponza
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f));
	s_Bloom->setMat4("model", model);
	u_Matrices->setData(0, (void*)glm::value_ptr(m_camera.getViewMatrix()), MAT4);
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	u_Matrices->setData(1, glm::value_ptr(proj), MAT4);

	o_Sponza->Draw(*s_Bloom);

	// Lamps
	s_Lamp->Use();
	o_Cube->vertexArray->Bind();
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 4));

	// Blur Pass
	bool horizontal = true, first_iteration = true;
	QuadVA->Bind();
	if (renderMode != 0 && !m_isWireFrame)
	{
		GLCall(glViewport(0, 0, sWidth / BLUR_SCALE, sHeight / BLUR_SCALE));
		GLCall(glActiveTexture(GL_TEXTURE0));
		int amount = 10;
		s_Blur->Use();
		for (int i = 0; i < amount; i++)
		{
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]));
			s_Blur->setBool("horizontal", horizontal);
			GLCall(glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]));
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		GLCall(glViewport(0, 0, sWidth, sHeight));
	}
	// Final Pass
	if (m_isWireFrame)
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	s_Final->Use();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, colorBuffers[0]));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

}

void TestBloom::OnImGuiRender()
{
	ImGui::Checkbox("WireFrame", &m_isWireFrame);
	if (ImGui::SliderFloat("Exposure", &exposure, 0.01f, 1.0f, "%.3f"))
		s_Final->setFloat("exposure", exposure);
	if (ImGui::Button("Reset Exposure"))
	{
		exposure = 0.1f;
		s_Final->setFloat("exposure", exposure);
	}
	ImGui::Separator();
	ImGui::RadioButton("Normal", &renderMode, 0);
	ImGui::RadioButton("Bloom Filter", &renderMode, 1);
	ImGui::RadioButton("Bloom", &renderMode, 2);
	if (renderMode != old_renderMode)
	{
		old_renderMode = renderMode;
		s_Final->setInt("renderMode", renderMode);
	}
	if (ImGui::Button("ScreenShot"))
	{
		void* data = new char[sWidth * sHeight * 4];
		GLCall(glReadPixels(0, 0, sWidth, sHeight, GL_RGB, GL_UNSIGNED_BYTE, data));
		stbi_flip_vertically_on_write(1);
		if (stbi_write_png("screenshot.png", sWidth, sHeight, 3, data, sWidth * 3))
			std::cout << "Took Screenshot\n";
		else
			std::cout << "Failed to take screenshot\n";
		delete[] data;
	}
}

void TestBloom::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;

		genFrameBuffers();
	}
}

void TestBloom::genFrameBuffers()
{
	GLCall(glDeleteFramebuffers(1, &hdrFBO));
	GLCall(glDeleteRenderbuffers(1, &depthRBO));
	GLCall(glDeleteTextures(2, colorBuffers));
	GLCall(glDeleteFramebuffers(2, pingpongFBO));
	GLCall(glDeleteTextures(2, pingpongBuffer));

	// Scene Buffer
	GLCall(glGenFramebuffers(1, &hdrFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
	GLCall(glGenTextures(2, colorBuffers));
	for (int i = 0; i < 2; i++)
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, colorBuffers[i]));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, sWidth, sHeight, 0, GL_RGB, GL_FLOAT, NULL));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0));
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	GLCall(glDrawBuffers(2, attachments));
	// create and attach depth buffer (renderbuffer)
	GLCall(glGenRenderbuffers(1, &depthRBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, depthRBO));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sWidth, sHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	// Blur Buffer
	GLCall(glGenFramebuffers(2, pingpongFBO));
	GLCall(glGenTextures(2, pingpongBuffer));
	for (int i = 0; i < 2; i++)
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]));
		GLCall(glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, sWidth / BLUR_SCALE, sHeight / BLUR_SCALE, 0, GL_RGB, GL_FLOAT, NULL));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0));
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	}
}
