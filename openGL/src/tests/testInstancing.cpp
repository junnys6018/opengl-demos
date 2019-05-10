#include "testInstancing.h"
#include "debug.h"

TestInstancing::TestInstancing(Camera& cam, GLFWwindow* win, uint16_t instances)
	:m_camera(cam), m_window(win), m_drawQuads(false), m_isWireFrame(false), m_amount(instances)
{
	// Quad setup
	float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
	QuadVB = std::make_unique<VertexBuffer>(quadVertices, sizeof(quadVertices));
	QuadVA = std::unique_ptr<VertexArray>(new VertexArray(*QuadVB, GL_FLOAT, { 2,3 }));

	s_Quad = std::make_unique<Shader>("res/Shaders/QuadInstance.shader");

	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}
	QuadVA->Bind();
	GLCall(glGenBuffers(1, &offsetVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, offsetVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(translations), translations, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribDivisor(2, 1));
	// Skybox setup
	std::vector<std::string> faces = {
		"res/Textures/CubeMaps/DeepSpaceGreen/leftImage.png",
		"res/Textures/CubeMaps/DeepSpaceGreen/rightImage.png",
		"res/Textures/CubeMaps/DeepSpaceGreen/upImage.png",
		"res/Textures/CubeMaps/DeepSpaceGreen/downImage.png",
		"res/Textures/CubeMaps/DeepSpaceGreen/frontImage.png",
		"res/Textures/CubeMaps/DeepSpaceGreen/backImage.png"
	};

	m_skyBox = std::make_unique<CubeMap>(faces);
	// planet setup

	o_Planet = std::make_unique<Object>("res/Objects/planet/planet.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Rock = std::make_unique<Object>("res/Objects/planet/rock.obj.expanded", OBJECT_INIT_FLAGS_GEN_MIPMAP | OBJECT_INIT_FLAGS_GEN_TEXTURE);

	s_rock = std::make_unique<Shader>("res/Shaders/Rocks.shader");
	s_planet = std::make_unique<Shader>("res/Shaders/Planet.shader");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	s_planet->setMat4("model", model);

	// generate model mat4's
	std::vector<glm::mat4> modelMatrices;
	modelMatrices.reserve(m_amount);
	float radius = 80.0f;
	offset = 10.0f;
	for (unsigned int i = 0; i < m_amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)m_amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices.push_back(model);
	}
	o_Rock->vertexArray->Bind();
	GLCall(glGenBuffers(1, &modelVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, modelVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0));
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(4));
	GLCall(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(5));
	GLCall(glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(6));

	GLCall(glVertexAttribDivisor(3, 1));
	GLCall(glVertexAttribDivisor(4, 1));
	GLCall(glVertexAttribDivisor(5, 1));
	GLCall(glVertexAttribDivisor(6, 1));
	
	m_camera.setSpeed(0.5f);
	GLCall(glEnable(GL_DEPTH_TEST));
}

TestInstancing::~TestInstancing()
{
	GLCall(glDeleteBuffers(1, &offsetVBO));
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	m_camera.setSpeed(0.08f);
	m_camera.resetPos();
}

void TestInstancing::OnUpdate()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	if (m_drawQuads)
	{
		QuadVA->Bind();
		s_Quad->Use();
		GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100));
	}
	else
	{
		m_camera.move(m_window);

		glm::mat4 view = m_camera.getViewMatrix();

		glm::mat4 proj = glm::mat4(1.0f);
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		if (width != 0 && height != 0)
			proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(width) / height, 0.4f, 400.0f);

		s_rock->setMat4("VP", proj * view);
		s_planet->setMat4("VP", proj * view);

		o_Planet->Draw(*s_planet);

		// draw meteorites
		s_rock->Use();
		o_Rock->vertexArray->Bind();
		glDrawElementsInstanced(GL_TRIANGLES, o_Rock->indexBuffer->getCount(), GL_UNSIGNED_INT, (void*)0, m_amount);

		m_skyBox->Draw(proj * glm::mat4(glm::mat3(m_camera.getViewMatrix())));

	}
}

void TestInstancing::OnImGuiRender()
{
	ImGui::Checkbox("toggle mode", &m_drawQuads);
	if (ImGui::Checkbox("Wireframe Mode", &m_isWireFrame))
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, (m_isWireFrame ? GL_LINE : GL_FILL)));
}
