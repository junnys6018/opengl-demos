#include "testDepthTest.h"
#include "debug.h"

TestAdvancedGL::TestAdvancedGL(Base_Camera* cam, GLFWwindow* win)
	:camera(cam), window(win), func_type(GL_LESS), old_func_type(GL_LESS)
	,visDepBuf(false), visLinDepBuf(false), useStencil(false), transRender(false)
{
	float vertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	CubeVB = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
	CubeVA = std::unique_ptr<VertexArray>(new VertexArray(*CubeVB, GL_FLOAT, { 3,2 }));
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	FloorVB = std::make_unique<VertexBuffer>(planeVertices, sizeof(planeVertices));
	FloorVA = std::unique_ptr<VertexArray>(new VertexArray(*FloorVB, GL_FLOAT, { 3,2 }));
	float transVertices[] = {
		// positions			// texture Coords
		0.0f,  0.5f,  0.0f,		0.0f,  1.0f,
		0.0f, -0.5f,  0.0f,		0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,		1.0f,  0.0f,

		0.0f,  0.5f,  0.0f,		0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,		1.0f,  0.0f,
		1.0f,  0.5f,  0.0f,		1.0f,  1.0f
	};
	TransVB = std::make_unique<VertexBuffer>(transVertices, sizeof(transVertices));
	TransVA = std::unique_ptr<VertexArray>(new VertexArray(*TransVB, GL_FLOAT, { 3,2 }));

	transPos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	transPos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	transPos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	transPos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	transPos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	CrateTex = std::make_unique<Texture>("res/Textures/container.jpg");
	FloorTex = std::make_unique<Texture>("res/Textures/concrete.jpg");
	GrassTex = std::make_unique<Texture>("res/Textures/blending_transparent_window.png", GL_CLAMP_TO_BORDER);

	CrateTex->Bind(0);
	FloorTex->Bind(1);
	GrassTex->Bind(2);

	shader = std::make_unique<Shader>("res/Shaders/DepthTest.shader");
	stencilShader = std::make_unique<Shader>("res/Shaders/SingleColor.shader");

	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

}

TestAdvancedGL::~TestAdvancedGL()
{
	GLCall(glDepthFunc(GL_LESS));
	GLCall(glDisable(GL_STENCIL_TEST));
	GLCall(glDisable(GL_BLEND));
}

void TestAdvancedGL::OnUpdate()
{
	glStencilMask(~0);
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	// camera movement
	if (camera->handleWindowInput(window) && transRender)
		sortDist();
	// MVP matrix
	glm::mat4 view = camera->getViewMatrix();

	glm::mat4 proj = glm::mat4(1.0f);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	if (width != 0 && height != 0)
		proj = glm::perspective(glm::radians(camera->getFOV()), (float)(width) / height, 0.1f, 100.0f);

	GLCall(glEnable(GL_STENCIL_TEST));
	// floor
	GLCall(glStencilMask(0x00));
	FloorVA->Bind();
	shader->setInt("tex", 1);
	shader->setMat4("MVP", proj * view);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	// cubes
	GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF)); // all fragments should update the stencil buffer
	GLCall(glStencilMask(0xFF)); // enable writing to the stencil buffer
	CubeVA->Bind();
	shader->setInt("tex", 0);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.01f, -1.0f));
	shader->setMat4("MVP", proj * view * model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.01f, 0.0f));
	shader->setMat4("MVP", proj * view * model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	// outline
	if (useStencil)
	{
		GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
		GLCall(glStencilMask(0x00)); // disable writing to the stencil buffer
		GLCall(glDisable(GL_DEPTH_TEST)); // we dont want the floor overriding the outline
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.01f, -1.0f));
		model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
		stencilShader->setMat4("MVP", proj * view * model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.01f, 0.0f));
		model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
		stencilShader->setMat4("MVP", proj * view * model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glDisable(GL_STENCIL_TEST));
	}
	// transparent entities
	if (transRender)
	{
		TransVA->Bind();
		shader->setInt("tex", 2);
		for (auto it = transPos.begin(); it != transPos.end(); ++it)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, *it);
			shader->setMat4("MVP", proj * view * model);
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
		}
	}
}

void TestAdvancedGL::sortDist()
{
	glm::vec3 camPos = camera->getCameraPos();
	std::sort(transPos.begin(), transPos.end(), [&camPos](const glm::vec3 & v1, const glm::vec3 & v2)->bool {
		return glm::length2(camPos - v1) >= glm::length2(camPos - v2);
		});
}

void TestAdvancedGL::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Depth Test Function"))
	{
		ImGui::RadioButton("GL_ALWAYS"  , &func_type, GL_ALWAYS  );
		ImGui::RadioButton("GL_NEVER"   , &func_type, GL_NEVER   );
		ImGui::RadioButton("GL_LESS"    , &func_type, GL_LESS    );
		ImGui::RadioButton("GL_EQUAL"   , &func_type, GL_EQUAL   );
		ImGui::RadioButton("GL_LEQUAL"  , &func_type, GL_LEQUAL  );
		ImGui::RadioButton("GL_GREATER" , &func_type, GL_GREATER );
		ImGui::RadioButton("GL_NOTEQUAL", &func_type, GL_NOTEQUAL);
		ImGui::RadioButton("GL_GEQUAL"  , &func_type, GL_GEQUAL  );	
		// only update func_type if it changed
		if (func_type != old_func_type)
		{
			old_func_type = func_type;
			GLCall(glDepthFunc(func_type));
		}
	}
	// clean up this code
	if (ImGui::Checkbox("Visualise Depth Buffer", &visDepBuf))
	{
		shader->setBool("visDepBuf", visDepBuf);
		visLinDepBuf = false;
		shader->setBool("visLinDepBuf", visLinDepBuf);
	}
	if (ImGui::Checkbox("Visualise Linear Depth Buffer", &visLinDepBuf))
	{
		shader->setBool("visLinDepBuf", visLinDepBuf);
		visDepBuf = false;
		shader->setBool("visDepBuf", visDepBuf);
	}
	ImGui::Checkbox("Stencil Testing", &useStencil);
	if (ImGui::Checkbox("Render Transparent Objects", &transRender))
		sortDist();
}
