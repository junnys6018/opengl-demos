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
TestDeferred::TestDeferred(Camera& cam, GLFWwindow* win, uint16_t nr_lights)
	:m_camera(cam), m_window(win), renderMode(0), old_renderMode(0), renderLights(true),
	useTileBased(true), visualiseLights(false), NR_LIGHTS(nr_lights), exposure(0.5f)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);
	genFrameBuffers();

	s_GeometryPass = std::make_unique<Shader>("res/shaders/DeferredShading/GeometryPass.shader");
	s_LightPass = std::make_unique<Shader>("res/shaders/DeferredShading/LightPass.shader");
	s_LightPass->setInt("gPosition", 0);
	s_LightPass->setInt("gNormal", 1);
	s_LightPass->setInt("gAlbedoSpec", 2);
	s_LightPass->setInt("NUM_LIGHTS", NR_LIGHTS);
	s_LightPass->setFloat("exposure", exposure);
	s_Lamp = std::make_unique<Shader>("res/shaders/DeferredShading/Lamp4.shader");
	s_FustrumCull = std::make_unique<Shader>("res/shaders/DeferredShading/cFustrumCull.shader");
	s_FustrumCull->setInt("gPosition", 0);
	s_FustrumCull->setInt("gNormal", 1);
	s_FustrumCull->setInt("gAlbedoSpec", 2);
	s_FustrumCull->setVec2("resolution", sWidth, sHeight);
	s_FustrumCull->setInt("NUM_LIGHTS", NR_LIGHTS);
	s_FustrumCull->setFloat("exposure", exposure);
	s_FinalPass = std::make_unique<Shader>("res/shaders/DeferredShading/FinalPass.shader");
	s_FinalPass->setInt("Texture", 0);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f));
	s_GeometryPass->setMat4("model", model);

	o_Sponza = std::make_unique<Object>("res/Objects/sponza/sponza.obj.expanded", OBJECT_INIT_FLAGS_GEN_TEXTURE | OBJECT_INIT_FLAGS_GEN_SPECULAR);
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

	u_Matrix = std::unique_ptr<UniformBuffer>(new UniformBuffer({ MAT4,MAT4 }));
	u_Matrix->Bind(1);
	// Generate Lights
	std::vector<PointLight> vecLight;
	vecLight.reserve(NR_LIGHTS);
	genLightsInCylinder(vecLight, 12, 12);
	//genLightsInRect(vecLight, glm::vec3(3, 6, 3), glm::vec3(0, 3, 0));
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
	// Generate model matrices for lights
	std::vector<glm::mat4> modelMatrices;
	modelMatrices.reserve(NR_LIGHTS);
	for (int i = 0; i < vecLight.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(vecLight[i].position));
		model = glm::scale(model, glm::vec3(0.1f));
		modelMatrices.push_back(model);
	}
	o_Cube->vertexArray->Bind();
	GLCall(glGenBuffers(1, &modelVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, modelVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, NR_LIGHTS * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW));

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
	o_Cube->vertexArray->unBind();
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_CULL_FACE));
}

TestDeferred::~TestDeferred()
{
	GLCall(glDeleteFramebuffers(1, &gBuffer));
	GLCall(glDeleteTextures(1, &gPosition));
	GLCall(glDeleteTextures(1, &gNormal));
	GLCall(glDeleteTextures(1, &gColorSpec));
	GLCall(glDeleteRenderbuffers(1, &rboDepth));
	GLCall(glDeleteTextures(1, &tex_output));

	GLCall(glDisable(GL_CULL_FACE));
}

void TestDeferred::OnUpdate()
{
	if (m_camera.move(m_window))
	{
		s_LightPass->setVec3("camPos", m_camera.getCameraPos());
		s_FustrumCull->setVec3("camPos", m_camera.getCameraPos());
	}
	// Geometry Pass
	timer[0].begin();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, gBuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera.m_FOV), (float)(sWidth) / sHeight, 0.1f, 100.0f);

	u_Matrix->setData(0, glm::value_ptr(view), MAT4);
	u_Matrix->setData(1, glm::value_ptr(proj), MAT4);

	o_Sponza->Draw(*s_GeometryPass, DRAW_FLAGS_DIFFUSE | DRAW_FLAGS_SPECULAR);
	timer[0].end();
	// Lighting Pass
	timer[1].begin();
	if (useTileBased && renderMode == 0)
	{
		s_FustrumCull->Use();

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, gPosition));
		GLCall(glActiveTexture(GL_TEXTURE1));
		GLCall(glBindTexture(GL_TEXTURE_2D, gNormal));
		GLCall(glActiveTexture(GL_TEXTURE2));
		GLCall(glBindTexture(GL_TEXTURE_2D, gColorSpec));
		u_Matrix->Bind(1);
		GLCall(glDispatchCompute(sWidth / 16, sHeight / 16, 1));

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, tex_output));
		QuadVA->Bind();
		s_FinalPass->Use();
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	}
	else
	{
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
	timer[1].end();
	timer[2].begin();
	if (renderLights)
	{
		// Draw Lamps (Done in forward rendering)
		o_Cube->vertexArray->Bind();
		s_Lamp->Use();
		GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer));
		GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0)); // write to default framebuffer
		GLCall(glBlitFramebuffer(0, 0, sWidth, sHeight, 0, 0, sWidth, sHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glDrawElementsInstanced(GL_TRIANGLES, o_Cube->indexBuffer->getCount(), GL_UNSIGNED_INT, 0, NR_LIGHTS));
	}
	timer[2].end();
}

void TestDeferred::OnImGuiRender()
{
	ImGui::Text("Number of Point Lights: %i", NR_LIGHTS);
	ImGui::Checkbox("Use Tile Based Deferred Shading", &useTileBased);
	if (useTileBased && ImGui::Checkbox("Visualise Lights", &visualiseLights))
		s_FustrumCull->setBool("visualiseLights", visualiseLights);
	ImGui::Checkbox("Render Lamps", &renderLights);
	ImGui::Separator();
	ImGui::RadioButton("Show Lighting Only", &renderMode, 0);
	ImGui::RadioButton("Show Position Only", &renderMode, 1);
	ImGui::RadioButton("Show Normal Only", &renderMode, 2);
	ImGui::RadioButton("Show Albedo Only", &renderMode, 3);
	ImGui::RadioButton("Show Specular Only", &renderMode, 4);
	ImGui::Separator();
	if (renderMode != old_renderMode)
	{
		old_renderMode = renderMode;
		s_LightPass->setInt("renderMode", renderMode);
	}

	if (ImGui::SliderFloat("Exposure", &exposure, 0.1, 5, "%.2f"))
	{
		s_LightPass->setFloat("exposure", exposure);
		s_FustrumCull->setFloat("exposure", exposure);
	}
	if (ImGui::Button("Reset Exposure"))
	{
		exposure = 0.5f;
		s_LightPass->setFloat("exposure", exposure);
		s_FustrumCull->setFloat("exposure", exposure);
	}
	if (ImGui::CollapsingHeader("Profiling"))
	{
		ImGui::Text("Geometry Pass: %.3f ms"  , (float)timer[0].getTime() / 1.0e6f);
		ImGui::Text("Deffered Pass: %.3f ms"  , (float)timer[1].getTime() / 1.0e6f);
		ImGui::Text("Lamp Pass: %.3f ms"      , (float)timer[2].getTime() / 1.0e6f);
		ImGui::Text("Total GPU Time: %.3f ms" ,
			(float)(timer[0].getTime() + timer[1].getTime() + timer[2].getTime()) / 1.0e6f);
		ImGui::Separator();
	}

	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);
	bool isSizeMod16 = (width % 16 == 0) && (height % 16 == 0);
	ImGui::Text("Width: %i, Height: %i, isSizeMod16 ? %s", width, height, isSizeMod16 ? "true" : "false");
}

void TestDeferred::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		// Screensize needs to be a multiple of 16 x 16
		sWidth = width - width % 16;
		sHeight = height - height % 16;
		s_FustrumCull->setVec2("resolution", sWidth, sHeight);
		glfwSetWindowSize(m_window, sWidth, sHeight);
		GLCall(glViewport(0, 0, sWidth, sHeight));
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
	GLCall(glDeleteTextures(1, &tex_output));

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
	GLCall(glGenRenderbuffers(1, &rboDepth));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboDepth));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sWidth, sHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth));


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	// Texture output
	GLCall(glGenTextures(1, &tex_output));
	GLCall(glBindTexture(GL_TEXTURE_2D, tex_output));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sWidth, sHeight, 0, GL_RGBA, GL_FLOAT, nullptr));
	GLCall(glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F));
}

void TestDeferred::genLightsInCylinder(std::vector<PointLight>& vecLight, float RADIUS, float HEIGHT)
{
	srand((unsigned int)glfwGetTime());
	for (int i = 0; i < NR_LIGHTS; i++)
	{
		float angle = rand() % 360;
		float height = (float)rand() / RAND_MAX * HEIGHT;
		float radius = (float)rand() / RAND_MAX * RADIUS;
		glm::vec4 position = glm::vec4(radius * sinf(glm::radians(angle)), height, radius * cosf(glm::radians(angle)), 1.0f);
		//	printf("x: %.3f y: %.3f z: %.3f \n", position.x, position.y, position.z);
		genLightAttibute(vecLight, position);
	}
}

void TestDeferred::genLightsInRect(std::vector<PointLight>& vecLight, glm::vec3 dimensions, glm::vec3 center)
{
	srand((unsigned int)glfwGetTime());
	for (int i = 0; i < NR_LIGHTS; i++)
	{
		glm::vec3 min = center - 0.5f * dimensions;
		glm::vec3 max = center + 0.5f * dimensions;
		// Generate random numbers with uniform distribution in [ min,max ]
		float x = min.x + (float)rand() / RAND_MAX * (max.x - min.x);
		float y = min.y + (float)rand() / RAND_MAX * (max.y - min.y);
		float z = min.z + (float)rand() / RAND_MAX * (max.z - min.z);
		glm::vec4 position = glm::vec4(x, y, z, 1.0f);
		genLightAttibute(vecLight, position);
	}
}

void genLightAttibute(std::vector<PointLight>& vecLight, glm::vec4 position)
{
	glm::vec4 color = glm::vec4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1.0f);
	float maxColor = std::max(color.r, std::max(color.g, color.b));
	color /= maxColor; // Normalize color such that max component = 1
	float intensity = (float)rand() / RAND_MAX * 4 + 1; // intensity ranges from 1 - 5
	float Lightradius = (-LINEAR + sqrtf(LINEAR * LINEAR - 4.0f * QUADRATIC * (CONSTANT - intensity * 256))) / (2 * QUADRATIC);
	vecLight.emplace_back(position, color, intensity, Lightradius);
}
