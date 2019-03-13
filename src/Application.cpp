#define GLEW_BUILD
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "debug.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void printBasicInfo()
{
	GLCall(glEnable(GL_DEPTH_TEST));
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << glGetString(GL_VERSION) << '\n';
	int numAttributes;
	GLCall(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes));
	std::cout << "Maximum # of vertex attributes supported: " << numAttributes << '\n';
}
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW init error\n";

	printBasicInfo();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Setup square vertices
	float vertices[] = {
		 0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	VertexBuffer vertex(vertices, sizeof(vertices));

	unsigned int indexBuffer[] = {
		0,1,2,
		2,3,0
	};
	IndexBuffer index(indexBuffer, 6);

	VertexArray SquareVA(vertex, GL_FLOAT, { 2,3,2 });
	SquareVA.addIndexBuffer(index);

	// Setup cube vertices
	float cube[] = {
	//    POSITION		  TEX_COORD
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

	VertexBuffer cubebuf(cube, sizeof(cube));
	VertexArray CubeVA(cubebuf, GL_FLOAT, { 3,2 });
	// SHADERS
	Shader exampleShader("res/shaders/Example.shader");
	Shader cubeShader("res/shaders/Cube.shader");
	// TEXTURES
	Texture chris512("res/Textures/Chris512.jpg");
	Texture chris567("res/Textures/Chris567.jpg");
//	Texture chris571win("res/Textures/Chris571win.jpg");
	Texture chris573("res/Textures/Chris573.jpg");
		Texture face573("res/Textures/awesomeface573.png", GL_RGBA);
//		Texture faceNET("res/Textures/awesomeface573paint.png", GL_RGBA);
	chris512.Bind(0);
	chris567.Bind(1);
//	chris571win.Bind(2);
	chris573.Bind(3);
		face573.Bind(4);
//		faceNET.Bind(5);
	// Set shader uniforms
	cubeShader.setInt("Texture1", 3);
	// Pre render loop preperation
	glfwSwapInterval(1);
	float theta = 0.0;
	float fov = 45.0f;
	CubeVA.Bind();
	cubeShader.Use();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// input
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && fov < 170.0f)
			fov += 0.3f;
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && fov > 10.0f)
			fov -= 0.3f;

		// MVP matrix
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
			model = glm::rotate(model, glm::radians(theta), glm::vec3(0.5, 0.3, 0.0f));
			cubeShader.setMat4("model", model);
			if (theta > 360.0)
				theta = 1.0f;
			else
				theta += 1.0f;
		}
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		cubeShader.setMat4("view", view);

		glm::mat4 proj = glm::mat4(1.0f);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		proj = glm::perspective(glm::radians(fov), (float)(width) / height, 0.1f, 100.0f);
		cubeShader.setMat4("proj", proj);

		/* Render here */
		GLCall(glClearColor(0.1f, 0.3f, 0.4f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}