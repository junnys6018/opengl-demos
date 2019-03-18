#define GLEW_BUILD
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <STB_IMAGE/stb_image.h>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "debug.h"
#include "Object.h"
#include "Camera.h"
// Instantiate camera object
Camera camera;
bool isRotating = true; // TODO: REMOVE GLOBAL VARIABLE

#include "callback.h"

void printBasicInfo()
{
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
	// Set callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetKeyCallback(window, key_callback);

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
	
	// Setup sphere vertices
	Object sphere("res/Objects/sphere.obj");
	VertexArray sphereVA(sphere.vertexBuffer, GL_FLOAT, { 3 });
	sphereVA.addIndexBuffer(sphere.indexBuffer);

	// SHADERS
	Shader simpleShader("res/Shaders/Simple.shader");
	Shader cubeShader("res/shaders/Cube.shader");

	// TEXTURES
	Texture chris512("res/Textures/Chris512.jpg");
	chris512.Bind(0);
	// Set shader uniforms
	cubeShader.setInt("Texture1", 0);

	// Pre render loop preperation
	GLCall(glEnable(GL_DEPTH_TEST));
	glfwSwapInterval(1);	// vsync

	float theta = 0.0;	// angle of rotation for cube
	float fov = 45.0f;

	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Input */
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && fov < 170.0f)
			fov += 0.3f;
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && fov > 10.0f)
			fov -= 0.3f;

		// camera movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.move(GLFW_KEY_W);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.move(GLFW_KEY_S);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.move(GLFW_KEY_A);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.move(GLFW_KEY_D);

		// if key 3 is pressed cubes will not rotate
		if (isRotating)
		{
			if (theta > 360.0)
				theta = 1.0f;
			else
				theta += 1.0f;
		}

		/* MVP matrix init */
		glm::mat4 model_cube = glm::mat4(1.0f);
		model_cube = glm::rotate(model_cube, glm::radians(theta), glm::vec3(0.5f, 0.3f, 0.0f));

		glm::mat4 model_sphere = glm::mat4(1.0f);
		model_sphere = glm::translate(model_sphere, glm::vec3(2.0f, 0.0f, 2.0f));
		model_sphere = glm::rotate(model_sphere, glm::radians(theta), glm::vec3(0.5f, 0.3f, 0.0f));

		glm::mat4 view;
		view = camera.getViewMatrix();

		glm::mat4 proj = glm::mat4(1.0f);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		proj = glm::perspective(glm::radians(fov), (float)(width) / height, 0.1f, 100.0f);

		/* Render here */
		GLCall(glClearColor(0.1f, 0.3f, 0.4f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		simpleShader.Use();
		simpleShader.setMat4("MVP", proj * view * model_sphere);
		sphereVA.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, sphere.indexBuffer.getCount(), GL_UNSIGNED_INT, 0));

		cubeShader.Use();
		cubeShader.setMat4("MVP", proj * view * model_cube);
		CubeVA.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}