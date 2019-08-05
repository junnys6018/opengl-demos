#include "testAdvGLSL.h"
#include "debug.h"

TestAdvGLSL::TestAdvGLSL(Base_Camera* cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), timePoint(0.0f), m_isWireFrame(false), m_hasNormals(false)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);

	s_glPointSize = std::make_unique<Shader>("res/Shaders/glPointSize.shader");
	s_glFragCoord = std::make_unique<Shader>("res/Shaders/glFragCoord.shader");
	s_glFrontFacing = std::make_unique<Shader>("res/Shaders/glFrontFacing.shader");
	s_glFragCoord->setVec2("screenSize", sWidth, sHeight);
	s_ExplodeShader = std::make_unique<Shader>("res/shaders/Explode.shader");
	s_NormVisualise = std::make_unique<Shader>("res/Shaders/NormVisualize.shader");
	s_Nanosuit = std::make_unique<Shader>("res/Shaders/Nidoqueen.shader");

	o_Cube = std::make_unique<Object>("res/Objects/cube.obj");
	o_Blastoise = std::make_unique<Object>("res/Objects/Pokemon/Blastoise/Blastoise.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);
	o_Nanosuit = std::make_unique<Object>("res/Objects/nanosuit/nanosuit.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	m_front = std::make_unique<Texture>("res/Textures/container.jpg");
	m_back = std::make_unique<Texture>("res/Textures/concrete.jpg");

	s_glFrontFacing->setInt("frontTexture", 1);
	s_glFrontFacing->setInt("backTexture", 2);
	m_front->Bind(1);
	m_back->Bind(2);
	// model mat init
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	s_glPointSize->setMat4("model", model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	s_glFragCoord->setMat4("model", model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	s_glFrontFacing->setMat4("model", model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -0.5f, -7.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	s_ExplodeShader->setMat4("model", model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, -0.5f, -7.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	s_Nanosuit->setMat4("model", model);
	s_NormVisualise->setMat4("model", model);


	// uniform buffer
	GLCall(glGenBuffers(1, &UBO));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, UBO));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW));
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO));

	// config
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_PROGRAM_POINT_SIZE));
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));

}

TestAdvGLSL::~TestAdvGLSL()
{
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_PROGRAM_POINT_SIZE));
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	GLCall(glDeleteBuffers(1, &UBO));
}

void TestAdvGLSL::OnUpdate()
{
	m_camera->handleWindowInput(m_window);

	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	// Render
	glm::mat4 view = m_camera->getViewMatrix();

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(m_camera->getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
	// gl_PointSize demo
	o_Cube->vertexArray->Bind();
	s_glPointSize->Use();
	GLCall(glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0));
	// gl_FragCoord demo
	o_Cube->Draw(*s_glFragCoord);

	// gl_FrontFacing demo
	o_Cube->Draw(*s_glFrontFacing);
	//o_Cube->Draw(*s_NormVisualise);

	// geometry shader demo
	currTime = glfwGetTime();
	float delta = currTime - lastTime;
	lastTime = currTime;
	if (m_isplaying)
	{
		timePoint += delta;
		if (timePoint > 6.28318530718f)
			timePoint = 0.0f;
		s_ExplodeShader->setFloat("time", (1.0f - cosf(timePoint)) / 2.0f);
	}
	o_Blastoise->Draw(*s_ExplodeShader);

	o_Nanosuit->Draw(*s_Nanosuit);
	if (m_hasNormals)
		o_Nanosuit->Draw(*s_NormVisualise, DRAW_FLAGS_NONE);
}

void TestAdvGLSL::OnImGuiRender()
{
	if (ImGui::Checkbox("Wireframe Mode", &m_isWireFrame))
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, (m_isWireFrame ? GL_LINE : GL_FILL)));
	if (ImGui::SliderFloat("Offset", &timePoint, 0.0f, 6.28318530718f, "%.3f"))
		s_ExplodeShader->setFloat("time", (1.0f - cosf(timePoint)) / 2.0f);
	ImGui::Checkbox("play", &m_isplaying);
	ImGui::SameLine();
	ImGui::Checkbox("normals", &m_hasNormals);
}

void TestAdvGLSL::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
		s_glFragCoord->setVec2("screenSize", width, height);
	}
}
