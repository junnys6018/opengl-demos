#include "testIBL_PBR.h"
#include "debug.h"

TestIBL_PBR::TestIBL_PBR(Base_Camera* cam, GLFWwindow* win, const std::string& hdrPath)
	:m_camera(cam), m_window(win), metalness(0.0f), roughness(0.0f), mipLevel(0.0), albedo(1.00, 0.71, 0.07),
	renderFlags(14), renderMode(1), oldRenderMode(1), skyboxTarget(4), oldSkyboxTarget(4)
{
	glfwGetFramebufferSize(m_window, &sWidth, &sHeight);

	o_Sphere = std::make_unique<Object>("res/Objects/sphere.obj");
	o_Cylinder = std::make_unique<Object>("res/Objects/cylinder.obj", OBJECT_INIT_FLAGS_GEN_TANGENT);
	o_Cerberus = std::make_unique<Object>("res/Objects/Cerberus/Cerberus.obj.expanded", OBJECT_INIT_FLAGS_GEN_TANGENT);

	s_Shader = std::make_unique<Shader>("res/shaders/PBR/IBL.shader");
	s_Shader->setVec3("viewPos", m_camera->getCameraPos());
	s_Shader->setInt("Albedo", 0);
	s_Shader->setInt("Normal", 1);
	s_Shader->setInt("Metalness", 2);
	s_Shader->setInt("Roughness", 3);
	s_Shader->setVec3("albedo_", albedo);
	s_Shader->setInt("irradianceMap", 5);
	s_Shader->setInt("prefilterMap", 6);
	s_Shader->setInt("brdfLUT", 7);
	s_Shader->setInt("renderMode", renderMode);

	s_SkyBox = std::make_unique<Shader>("res/shaders/PBR/SkyBox.shader");
	s_SkyBox->setInt("skybox", 4);
	// Load Radiance HDR map
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(hdrPath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLCall(glGenTextures(1, &hdrTexture));
		GLCall(glBindTexture(GL_TEXTURE_2D, hdrTexture));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		stbi_image_free(data);
	}
	else std::cout << "Failed to load HDR image\n";

	/* DIFFUSE PRECOMPUTING */

	// Convert Radiance HDR map to cubemap
	o_Cube = std::make_unique<Object>("res/Objects/cube.obj");
	s_Equ2Cube = std::make_unique<Shader>("res/shaders/PBR/Equirectangular2CubeMap.shader");
	s_Equ2Cube->setInt("equirectangularMap", 1);
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, hdrTexture));
	// Capture HDR map to Framebuffer
	GLCall(glGenFramebuffers(1, &CaptureFBO));
	GLCall(glGenRenderbuffers(1, &CaptureRBO));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, CaptureFBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, CaptureRBO));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, FBOwidth, FBOwidth));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, CaptureRBO));

	allocate_mem_to_cube_map(&EnviromentMap, FBOwidth);
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)); // To sample mipmap levels
	render_to_cube_map(&EnviromentMap, FBOwidth, s_Equ2Cube);
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, EnviromentMap));
	GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

	// Convolute Environment cubemap into Irradiance cubemap
	s_Convolution = std::make_unique<Shader>("res/shaders/PBR/Convolution.shader");
	s_Convolution->setInt("environmentMap", 1);
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, EnviromentMap));
	GLCall(glActiveTexture(GL_TEXTURE0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, CaptureFBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, CaptureRBO));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32));

	allocate_mem_to_cube_map(&IrradianceMap, 32);
	render_to_cube_map(&IrradianceMap, 32, s_Convolution);

	/* SPECULAR PRECOMPUTING */

	allocate_mem_to_cube_map(&PrefilterMap, 128);
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

	s_PreFilter = std::make_unique<Shader>("res/shaders/PBR/PreFilter.shader");
	s_PreFilter->setInt("environmentMap", 1);
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, EnviromentMap));
	GLCall(glActiveTexture(GL_TEXTURE0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, CaptureFBO));
	const int MIP_LEVELS = 5;
	for (int mip = 0; mip < MIP_LEVELS; mip++)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, CaptureRBO));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipWidth));
		float roughness = (float)mip / (float)(MIP_LEVELS - 1);
		s_PreFilter->setFloat("roughness", roughness);

		render_to_cube_map(&PrefilterMap, mipWidth, s_PreFilter, mip);
	}

	/* BINDINGS */

	// Bind default framebuffer
	GLCall(glViewport(0, 0, sWidth, sHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	// Bind Textures
	generateTextures("res/Textures/PBR/ivory_panel", 0);
	generateTextures("res/Objects/Cerberus/Textures", 1);
	GLCall(glActiveTexture(GL_TEXTURE4));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, EnviromentMap));
	GLCall(glActiveTexture(GL_TEXTURE5));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap));
	GLCall(glActiveTexture(GL_TEXTURE6));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap));
	t_brdfLUT = std::make_unique<Texture>("res/Textures/PBR/brdftexture.png");
	t_brdfLUT->Bind(7);

	GLCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LEQUAL));
}
void TestIBL_PBR::allocate_mem_to_cube_map(unsigned int* cubemap, int width)
{
	// Allocate memory for cubemap
	GLCall(glGenTextures(1, cubemap));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemap));
	for (int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			width, width, 0, GL_RGB, GL_FLOAT, nullptr));
	}
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}
void TestIBL_PBR::render_to_cube_map(unsigned int* cubemap, int width, std::unique_ptr<Shader>& shader, int mip)
{
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemap));
	// Capture shader output onto the cubemap faces
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	shader->setMat4("proj", captureProjection);
	GLCall(glViewport(0, 0, width, width)); // don't forget to configure the viewport to the capture dimensions.
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, CaptureFBO));
	for (int i = 0; i < 6; i++)
	{
		shader->setMat4("view", captureViews[i]);
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *cubemap, mip));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		o_Cube->Draw(*shader);
	}
}

TestIBL_PBR::~TestIBL_PBR()
{
	GLCall(glDeleteTextures(1, &hdrTexture));
	GLCall(glDeleteTextures(1, &EnviromentMap));
	GLCall(glDeleteTextures(1, &IrradianceMap));
	GLCall(glDeleteTextures(1, &PrefilterMap));

	GLCall(glDeleteFramebuffers(1, &CaptureFBO));
	GLCall(glDeleteRenderbuffers(1, &CaptureRBO));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	GLCall(glDepthFunc(GL_LESS));
}

void TestIBL_PBR::OnUpdate()
{
	timer.begin();
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	if (m_camera->handleWindowInput(m_window))
	{
		s_Shader->setVec3("viewPos", m_camera->getCameraPos());
	}
	glm::mat4 view = m_camera->getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(m_camera->getFOV()), (float)(sWidth) / sHeight, 0.1f, 100.0f);
	s_Shader->setMat4("VP", proj * view);
	// Spheres
	if (renderFlags & (1 << 1))
	{
		s_Shader->setBool("useSamplers", false);
		for (int i = 1; i <= 6; i++)
		{
			for (int j = 1; j <= 6; j++)
			{
				s_Shader->setFloat("metalness", (float)(i - 1) / 5.0f);
				s_Shader->setFloat("roughness", (float)(j - 1) / 5.0f);
				glm::mat4 model = glm::mat4(1.0f);
				glm::vec3 offset = glm::vec3(0.0f, (1.0f + 6.0f) / 2.0f, (1.0f + 6.0f) / 2.0f);
				model = glm::translate(model, glm::vec3(4.0f, i * 1.0f, j * 1.0f) - offset);
				model = glm::scale(model, glm::vec3(0.3f));
				s_Shader->setMat4("model", model);
				o_Sphere->Draw(*s_Shader, DRAW_FLAGS_TRIANGLE_STRIP);
			}
		}

		s_Shader->setFloat("metalness", metalness);
		s_Shader->setFloat("roughness", roughness);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 6.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		s_Shader->setMat4("model", model);
		o_Sphere->Draw(*s_Shader, DRAW_FLAGS_TRIANGLE_STRIP);

		s_Shader->setBool("useSamplers", true);
	}
	// Cylinder
	if (renderFlags & (1 << 2))
	{
		bindTextures(0);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.05f * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		s_Shader->setMat4("model", model);
		o_Cylinder->Draw(*s_Shader);
	}
	// Cerberus
	if (renderFlags & (1 << 3))
	{
		bindTextures(1);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 2.5f));
		model = glm::scale(model, glm::vec3(1.8f));
		s_Shader->setMat4("model", model);
		o_Cerberus->Draw(*s_Shader);
	}

	s_SkyBox->setMat4("VP", proj * glm::mat4(glm::mat3(view)));
	o_Cube->Draw(*s_SkyBox);

	timer.end();
}

void TestIBL_PBR::OnImGuiRender()
{
	ImGui::Text("Render Meshes:");
	ImGui::CheckboxFlags("Spheres", &renderFlags, 1 << 1);
	ImGui::CheckboxFlags("Cylinder", &renderFlags, 1 << 2);
	ImGui::CheckboxFlags("Cerberus", &renderFlags, 1 << 3);

	ImGui::Separator();

	ImGui::Text("Skybox Target:");
	ImGui::RadioButton("Environment Map", &skyboxTarget, 4);
	ImGui::RadioButton("Irradiance Map", &skyboxTarget, 5);
	ImGui::RadioButton("PreFilter Map", &skyboxTarget, 6);
	if (skyboxTarget != oldSkyboxTarget)
	{
		oldSkyboxTarget = skyboxTarget;
		s_SkyBox->setInt("skybox", skyboxTarget);
	}
	if (skyboxTarget == 6 && ImGui::SliderFloat("Mip Level", &mipLevel, 0.0f, 4.0f)) // Prefilter map
	{
		s_SkyBox->setFloat("mip", mipLevel);
	}
	ImGui::Separator();

	ImGui::SliderFloat("Metalness", &metalness, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
	if (ImGui::ColorEdit3("Albedo", &albedo[0]))
	{
		s_Shader->setVec3("albedo_", albedo);
	}
	if (ImGui::Button("Gold", ImVec2(90, 25)))
		albedo = glm::vec3(1.00, 0.71, 0.07);
	ImGui::SameLine();
	if (ImGui::Button("Copper", ImVec2(90, 25)))
		albedo = glm::vec3(0.95, 0.64, 0.54);
	ImGui::SameLine();
	if (ImGui::Button("Iron", ImVec2(90, 25)))
		albedo = glm::vec3(0.56, 0.57, 0.58);

	s_Shader->setVec3("albedo_", albedo);
		
	ImGui::Separator();

	ImGui::Text("Render Target:");
	ImGui::RadioButton("Lighting", &renderMode, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Freshnel", &renderMode, 2);
	if (renderMode != oldRenderMode)
	{
		oldRenderMode = renderMode;
		s_Shader->setInt("renderMode", renderMode);
	}
	if (ImGui::CollapsingHeader("Textures"))
	{
		if (ImGui::Button("Future Panel", ImVec2(120.0f, 25.0f)))
			generateTextures("res/Textures/PBR/future_panel", 0);
		if (ImGui::Button("Ivory Panel", ImVec2(120.0f, 25.0f)))
			generateTextures("res/Textures/PBR/ivory_panel", 0);
		if (ImGui::Button("Bath Tile", ImVec2(120.0f, 25.0f)))
			generateTextures("res/Textures/PBR/bath_tile", 0);
		if (ImGui::Button("Mosaic", ImVec2(120.0f, 25.0f)))
			generateTextures("res/Textures/PBR/mosaic", 0);
		if (ImGui::Button("Subway Tile", ImVec2(120.0f, 25.0f)))
			generateTextures("res/Textures/PBR/subway_tile", 0);
	}
	ImGui::Text("Total GPU time: %.3f ms", timer.getTime() / 1.0e6f);
}

void TestIBL_PBR::framebuffer_size_callback(int width, int height)
{
	if (width != 0 && height != 0)
	{
		sWidth = width;
		sHeight = height;
	}
}

void TestIBL_PBR::generateTextures(const std::string& path, int index)
{
	t_Albedo[index].reset();
	t_Normal[index].reset();
	t_Metalness[index].reset();
	t_Roughness[index].reset();

	t_Albedo[index] = std::make_unique<Texture>(path + "/d.jpg");
	t_Normal[index] = std::make_unique<Texture>(path + "/n.jpg");
	t_Metalness[index] = std::make_unique<Texture>(path + "/m.jpg");
	t_Roughness[index] = std::make_unique<Texture>(path + "/r.jpg");

	bindTextures(index);
}

void TestIBL_PBR::bindTextures(int index)
{
	t_Albedo[index]->Bind(0);
	t_Normal[index]->Bind(1);
	t_Metalness[index]->Bind(2);
	t_Roughness[index]->Bind(3);
}
