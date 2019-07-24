#include "testLighting.h"
#include "debug.h"


TestLighting::TestLighting(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), m_theta(0.0f), m_isRotating(false), m_isSpotLighton(false)
	, m_dirLightColor(0.2f, 0.2f, 1.0f)
{
	// translation vectors for each cube
	m_cubePos = new glm::vec3[10];
	m_cubePos[0] = glm::vec3( 0.0f,  0.0f,  0.0f );
	m_cubePos[1] = glm::vec3( 2.0f,  5.0f, -15.0f);
	m_cubePos[2] = glm::vec3(-1.5f, -2.2f, -2.5f );
	m_cubePos[3] = glm::vec3(-3.8f, -2.0f, -12.3f);
	m_cubePos[4] = glm::vec3( 2.4f, -0.4f, -3.5f );
	m_cubePos[5] = glm::vec3(-1.7f,  3.0f, -7.5f );
	m_cubePos[6] = glm::vec3( 1.3f, -2.0f, -2.5f );
	m_cubePos[7] = glm::vec3( 1.5f,  2.0f, -2.5f );
	m_cubePos[8] = glm::vec3( 1.5f,  0.2f, -1.5f );
	m_cubePos[9] = glm::vec3(-1.3f,  1.0f, -1.5f );
	// cube setup
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	m_cubeBuf = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
	m_cubeVA = std::unique_ptr<VertexArray>(new VertexArray(*m_cubeBuf, GL_FLOAT, { 3,3,2 }));
	m_lampVA = std::unique_ptr<VertexArray>(new VertexArray(*m_cubeBuf, GL_FLOAT, { 3,3,2 }));

	m_lightingShader = std::make_unique<Shader>("res/Shaders/lighting.shader");
	m_lightingShader->Use();
	// load shader uniforms
	m_lightingShader->setInt("material.shininess", 32);
	m_diffMap = std::make_unique<Texture>("res/Textures/container2.png");
	m_lightingShader->setInt("material.diffuse", 0);
	m_specMap = std::make_unique<Texture>("res/Textures/container2_specular.png");
	m_lightingShader->setInt("material.specular", 1);
	m_emisMap = std::make_unique<Texture>("res/Textures/matrix.jpg");
	m_lightingShader->setInt("material.emission", 2);

	m_diffMap->Bind(0);
	m_specMap->Bind(1);
	m_emisMap->Bind(2);
	// directional lighting
	m_lightingShader->setVec3("dirLight.ambient", 0.2f * m_dirLightColor);
	m_lightingShader->setVec3("dirLight.diffuse", m_dirLightColor);
	m_lightingShader->setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
	// point lighting

	m_pointLightPositions = new glm::vec3[4];
	m_pointLightPositions[0] = glm::vec3(0.0f, 1.0f, 0.0f);
	m_pointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.0f);
	m_pointLightPositions[2] = glm::vec3(-4.0f, 2.0f, -12.0f);
	m_pointLightPositions[3] = glm::vec3(0.0f, 0.0f, -3.0f);

	for (int i = 0; i != 4; ++i)
	{
		m_lightingShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.14f, 0.06f, 0.04f);
		m_lightingShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.7f, 0.3f, 0.2f);
		m_lightingShader->setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		m_lightingShader->setFloat("pointLights[" + std::to_string(i)  + "].constant", 1.0f);
		m_lightingShader->setFloat("pointLights[" + std::to_string(i)  + "].lin", 0.09f);
		m_lightingShader->setFloat("pointLights[" + std::to_string(i)  + "].quadratic", 0.032f);
	}
	// spot lighting
	m_lightingShader->setFloat("spotLight.cutOff", cos(glm::radians(12.5f)));
	m_lightingShader->setFloat("spotLight.outerCutOff", cos(glm::radians(17.5f)));
	m_lightingShader->setVec3("spotLight.color", 0.4f, 0.3f, 0.3f);
	m_lightingShader->setFloat("spotLight.constant", 1.0f);
	m_lightingShader->setFloat("spotLight.lin", 0.09f);
	m_lightingShader->setFloat("spotLight.quadratic", 0.032f);
	m_lightingShader->setBool("spotLight.isOn", m_isSpotLighton);

	// shaders	
	m_lampShader = std::make_unique<Shader>("res/Shaders/lamp.shader");

	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glEnable(GL_DEPTH_TEST));

}
TestLighting::~TestLighting()
{
	delete[] m_cubePos;
	delete[] m_pointLightPositions;
}

void TestLighting::OnUpdate()
{
	// camera movement
	m_camera.move(m_window);
	// update light pos
	if (m_isRotating)
	{
		m_pointLightPositions[0].z = 4.0f * cos(m_theta / 57.0f) - 4.0f;
		m_theta += 1.0f;
	}

	// MVP matrix
	glm::mat4 view;
	view = m_camera.getViewMatrix();

	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(m_camera.getFOV()), (float)(width) / height, 0.1f, 100.0f);


	// render
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	// lamps
	m_lampShader->Use();
	m_lampVA->Bind();
	for (int i = 0; i != 4; ++i)
	{
		glm::mat4 model_lamp = glm::mat4(1.0f);
		model_lamp = glm::translate(model_lamp, m_pointLightPositions[i]);
		model_lamp = glm::scale(model_lamp, glm::vec3(0.2f));
		m_lampShader->setMat4("MVP", proj * view * model_lamp);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	}
	// objects
	m_lightingShader->Use();
	m_cubeVA->Bind();
	glm::vec3 direction = glm::mat3(view) * glm::vec3(-1.0f, -0.5f, 0.3f);
	m_lightingShader->setVec3("dirLight.direction", direction);
	for (int i = 0; i != 4; ++i)
		m_lightingShader->setVec3("pointLights[" + std::to_string(i) + "].position",
			view * glm::vec4(m_pointLightPositions[i], 1.0f));
	for (int i = 0; i != 10; ++i)
	{
		glm::mat4 model_cube = glm::mat4(1.0f);
		model_cube = glm::translate(model_cube, m_cubePos[i]);
		model_cube = glm::rotate(model_cube, glm::radians((float)(20 * i)), glm::vec3(1.0f, 0.3f, 0.5f));
		m_lightingShader->setMat4("MVP", proj * view * model_cube);
		m_lightingShader->setMat4("MV", view * model_cube);
		if (i % 3 == 0)
			m_lightingShader->setBool("useEmission", true);
		else m_lightingShader->setBool("useEmission", false);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	}
}	
void TestLighting::key_callback(int key, int action)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		m_isRotating = !m_isRotating;
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		m_isSpotLighton = !m_isSpotLighton;
		m_lightingShader->setBool("spotLight.isOn", m_isSpotLighton);
	}
}

void TestLighting::OnImGuiRender()
{
	ImGui::Checkbox("Move Light", &m_isRotating);
	ImGui::SameLine(); ImGui::Text(" (key: r)");
	if (ImGui::Checkbox("Spotlight", &m_isSpotLighton))
		m_lightingShader->setBool("spotLight.isOn", m_isSpotLighton);
	ImGui::SameLine(); ImGui::Text(" (key: l)");
	if (ImGui::ColorEdit3("Directional light color", &m_dirLightColor[0]))
	{
		m_lightingShader->setVec3("dirLight.ambient", 0.2f * m_dirLightColor);
		m_lightingShader->setVec3("dirLight.diffuse", m_dirLightColor);
	}
}