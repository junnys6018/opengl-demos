#include "testDeferred.h"
#include "debug.h"

#define CONSTANT 1.0f
#define LINEAR 0.14f
#define QUADRATIC 0.07f

struct PointLight
{
	glm::vec4 position;
	glm::vec4 color;
	float intensity;
	float radius;

	PointLight(glm::vec4 pos, glm::vec4 col, float intensity, float rad)
		:position(pos), color(col), intensity(intensity), radius(rad)	{}
};
TestDeferred::TestDeferred(Camera& cam, GLFWwindow* win)
	:m_camera(cam), m_window(win), renderMode(0), old_renderMode(0), NUM_LIGHTS(128)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	genFrameBuffers();

	s_GeometryPass = std::make_unique<Shader>("res/shaders/DeferredShading/GeometryPass.shader");
	s_LightPass = std::make_unique<Shader>("res/shaders/DeferredShading/LightPass.shader");
	s_LightPass->setInt("gPosition", 0);
	s_LightPass->setInt("gNormal", 1);
	s_LightPass->setInt("gAlbedoSpec", 2);
	s_LightPass->setInt("NUM_LIGHTS", NUM_LIGHTS);

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
	// Generate Lights in a cylinder of radius and height about (0,0,0)
	std::vector<PointLight> vecLight;
	vecLight.reserve(NUM_LIGHTS);
	const float RADIUS = 12.0f, HEIGHT = 12.0f; 
	srand((unsigned int)glfwGetTime());
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		float angle = rand() % 360;
		float height = (float)rand() / RAND_MAX * HEIGHT;
		float radius = (float)rand() / RAND_MAX * RADIUS;
		glm::vec4 position = glm::vec4(radius * sinf(glm::radians(angle)), height, radius * cosf(glm::radians(angle)), 1.0f);
		printf("x: %.3f y: %.3f z: %.3f \n", position.x, position.y, position.z);
		glm::vec4 color = glm::vec4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1.0f);
		float maxColor = std::max(color.r, std::max(color.g, color.b));
		color /= maxColor; // Normalize color s.t. max component = 1
		float intensity = (float)rand() / RAND_MAX * 4 + 1; // intensity ranges from 1 - 5
		float Lightradius = (-LINEAR + sqrtf(LINEAR * LINEAR - 4.0f * QUADRATIC * (CONSTANT - intensity * 256))) / (2 * QUADRATIC);
		vecLight.emplace_back(position, color, intensity, Lightradius);
	}
	GLCall(glGenBuffers(1, &lightsSSBO));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsSSBO));
	// sizeof(PointLight) = 48 by std140 rules
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, vecLight.size() * 48, nullptr, GL_STATIC_DRAW));
	for (int i = 0; i < vecLight.size(); i++)
	{
		GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 48 * i, 40, &vecLight[i]));
	}
	GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsSSBO));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
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
	s_LightPass->setVec3("camPos", m_camera.getCameraPos());
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
