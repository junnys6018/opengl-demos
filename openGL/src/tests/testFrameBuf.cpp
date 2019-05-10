#include "testFrameBuf.h"
#include "debug.h"

TestFrameBuf::TestFrameBuf(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_isWireFrame(false), m_camSpeed(1.0f), flags(0), oldFlags(0), m_convOffset(10.0f)
{
	oBlastoise = std::make_unique<Object>("res/Objects/Pokemon/Blastoise/Blastoise.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	oNidoqueen = std::make_unique<Object>("res/Objects/Pokemon/Nidoqueen/XY_Nidoqueen.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	oPikachu = std::make_unique<Object>("res/Objects/Pokemon/Pikachu/Pikachu.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	oPiplup = std::make_unique<Object>("res/Objects/Pokemon/Piplup/Piplup.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	m_shader = std::make_unique<Shader>("res/Shaders/Blastoise.shader");
	m_floorShader = std::make_unique<Shader>("res/Shaders/Cube.shader");
	m_FramebufShader = std::make_unique<Shader>("res/Shaders/FrameBuf.shader");
	m_FramebufShader->setInt("Texture", 1);
	m_FramebufShader->setFloat("offset", m_convOffset / 3000.0f);

	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.01f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.01f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.01f, -5.0f,  0.0f, 1.0f,

		 5.0f, -0.01f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.01f, -5.0f,  0.0f, 1.0f,
		 5.0f, -0.01f, -5.0f,  1.0f, 1.0f
	};
	FloorVB = std::make_unique<VertexBuffer>(planeVertices, sizeof(planeVertices));
	FloorVA = std::unique_ptr<VertexArray>(new VertexArray(*FloorVB, GL_FLOAT, { 3,2 }));
	FloorTex = std::make_unique<Texture>("res/Textures/battleground.jpg");
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

	glfwGetWindowSize(m_window, &sWidth, &sHeight);
	GenFrameBuffer(sWidth, sHeight);
}

TestFrameBuf::~TestFrameBuf()
{
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	m_camera.setSpeed(0.1f);

	GLCall(glDeleteFramebuffers(1, &FBO));
	GLCall(glDeleteTextures(1, &texture));
	GLCall(glDeleteRenderbuffers(1, &RBO));
}

void TestFrameBuf::OnUpdate()
{
	m_camera.move(m_window);

	// First Pass
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCall(glEnable(GL_DEPTH_TEST));

	GLCall(glPolygonMode(GL_FRONT_AND_BACK, (m_isWireFrame ? GL_LINE : GL_FILL)));

	glm::mat4 view = m_camera.getViewMatrix();

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	// Floor
	FloorVA->Bind();
	FloorTex->Bind(0);
	m_floorShader->setInt("Texture1", 0);
	m_floorShader->setMat4("MVP", proj * view);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	// Blastoise
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
	model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
	m_shader->setMat4("MVP", proj * view * model);
	oBlastoise->Draw(*m_shader);
	// Pikachu
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
	model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_shader->setMat4("MVP", proj * view * model);
	oPikachu->Draw(*m_shader);
	// Nidoqueen
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.012f, 0.012f, 0.012f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_shader->setMat4("MVP", proj * view * model);
	oNidoqueen->Draw(*m_shader);
	// Piplup
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_shader->setMat4("MVP", proj * view * model);
	oPiplup->Draw(*m_shader);

	// Second Pass
	
	GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
	GLCall(glDisable(GL_DEPTH_TEST));

	if (m_isWireFrame)
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	m_FramebufShader->Use();
	QuadVA->Bind();

	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void TestFrameBuf::OnImGuiRender()
{
	ImGui::Checkbox("Wireframe Mode", &m_isWireFrame);
	if (ImGui::SliderFloat("Cam Speed", &m_camSpeed, 0.0f, 3.0f))
		m_camera.setSpeed(m_camSpeed / 10.0f);
	if (ImGui::SliderFloat("kernel offset", &m_convOffset, 1, 50, "%.0f"))
	{
		m_FramebufShader->setFloat("offset", m_convOffset / 3000.0f);
	}
	if (ImGui::Button("Reset Offset"))
	{
		m_convOffset = 10.0f;
		m_FramebufShader->setFloat("offset", m_convOffset / 3000.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Speed"))
	{
		m_camSpeed = 1.0f;
		m_camera.setSpeed(m_camSpeed / 10.0f);
	}
	if (ImGui::CollapsingHeader("Post-processing effects"))
	{
		// clean up this code
		ImGui::RadioButton("no effect"     , &flags, 0);
		ImGui::RadioButton("inversion"     , &flags, 1);
		ImGui::RadioButton("grayscale"     , &flags, 2);
		ImGui::RadioButton("sharpen"       , &flags, 3);
		ImGui::RadioButton("blur"          , &flags, 4);
		ImGui::RadioButton("edge detection", &flags, 5);
		// only update flags if it changed
		if (flags != oldFlags)
		{
			oldFlags = flags;
			m_FramebufShader->setInt("flags", flags);
		}
	}
}

void TestFrameBuf::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
		GenFrameBuffer(width, height);
	}
}

bool TestFrameBuf::GenFrameBuffer(int width, int height)
{
	// Delete resources
	GLCall(glDeleteFramebuffers(1, &FBO));
	GLCall(glDeleteTextures(1, &texture));
	GLCall(glDeleteRenderbuffers(1, &RBO));

	// Framebuffer init
	GLCall(glGenFramebuffers(1, &FBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	// color attachment
	GLCall(glGenTextures(1, &texture));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));

	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture));
	// Renderbuffer
	GLCall(glGenRenderbuffers(1, &RBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO));
	//GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	//GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;

	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
