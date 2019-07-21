#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "debug.h"
#include "Camera.h"

#include "tests/__Test__.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Camera camera;
GLFWwindow* window;
TestManager test_mgr(camera, &window);
#include "callback.h"
int main(int argc, char* argv[])
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return -1;
	window = glfwCreateWindow(1080, 720, "OpenGL tests", NULL, NULL);
	test_mgr.registerTests(); // tests need to be registered after window is initilaised
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // enable vsync
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW init error\n";

	printBasicInfo();

	// Set callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// command line arg for default test
	if (argc == 2)
	{
		std::string command("/test ");
		command.append(argv[1]);
		test_mgr.parseInput(command);
	}
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Font loading
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("res/Fonts/Consola.ttf", 16);
	
	test_mgr.gameLoop();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}