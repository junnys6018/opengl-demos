#include "testCubeMap.h"
#include "debug.h"

TestCubeMap::TestCubeMap(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win)
{
	oBlastoise = std::make_unique<Object>("res/Objects/Pokemon/Blastoise/Blastoise.obj", OBJECT_INIT_FLAGS_GEN_TEXTURE);

	std::vector<std::string> faces = {
		"res/Textures/CubeMaps/clearSky/right.jpg",
		"res/Textures/CubeMaps/clearSky/left.jpg",
		"res/Textures/CubeMaps/clearSky/top.jpg",
		"res/Textures/CubeMaps/clearSky/bottom.jpg",
		"res/Textures/CubeMaps/clearSky/front.jpg",
		"res/Textures/CubeMaps/clearSky/back.jpg"
	};
	loadCubeMap(faces);
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	skyBoxVB = std::make_unique<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices));
	skyBoxVA = std::unique_ptr<VertexArray>(new VertexArray(*skyBoxVB, GL_FLOAT, { 3 }));

	m_shader = std::make_unique<Shader>("res/Shaders/CubeMap.shader");
	skyBoxShader = std::make_unique<Shader>("res/Shaders/SkyBox.shader");
	skyBoxShader->setInt("skybox", 1);
	m_shader->setInt("skybox", 1);
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(0.1f, 0.6f, 0.1f, 1.0f));
}

TestCubeMap::~TestCubeMap()
{
	GLCall(glDeleteTextures(1, &m_cubeMap));
}

void TestCubeMap::OnUpdate()
{
	m_camera.move(m_window);
	
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	// objects
	GLCall(glDepthFunc(GL_LESS));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.22f, 0.22f, 0.22f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(width) / height, 0.1f, 100.0f);

	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap));
	m_shader->Use();
	m_shader->setMat4("MVP", proj * view * model);
	m_shader->setMat4("model", model);
	m_shader->setVec3("camPos", m_camera.getCameraPos());
	oBlastoise->Draw(*m_shader);

	// skybox
	GLCall(glDepthFunc(GL_LEQUAL));

	skyBoxVA->Bind();
	skyBoxShader->Use();
	skyBoxShader->setMat4("VP", proj * glm::mat4(glm::mat3(m_camera.getViewMatrix())));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

}

void TestCubeMap::loadCubeMap(std::vector<std::string> faces)
{
	GLCall(glGenTextures(1, &m_cubeMap));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap));
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); ++i)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
