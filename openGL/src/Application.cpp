#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "debug.h"

#include "tests/__Test__.h"
#include "architecture/test_manager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLFWwindow* window;
TestManager test_mgr(&window);
#include "callback.h"

void ImGui_Init();
void ImGui_Shutdown();

int main(int argc, char* argv[])
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(1080, 720, "OpenGL tests", NULL, NULL);
	// Set Window Icon
	int width, height, nrChannels;
	unsigned char* data = stbi_load("res/icon.png", &width, &height, &nrChannels, 4);
	GLFWimage icon = { width,height,data };
	glfwSetWindowIcon(window, 1, &icon);
	stbi_image_free(data);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // enable vsync
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW init error\n";
	GLCall(glEnable(GL_MULTISAMPLE));
	printBasicInfo();

	// Set callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	ImGui_Init();
	
	test_mgr.gameLoop();

	ImGui_Shutdown();

	glfwTerminate();
	return 0;
}

void ImGui_Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Font loading
	io.Fonts->AddFontFromFileTTF("res/Fonts/Consola.ttf", 16);
	// Bind my window creation callback
	PrevWindowCreateCallback = ImGui::GetPlatformIO().Platform_CreateWindow;
	ImGui::GetPlatformIO().Platform_CreateWindow = OnWindowCreate;
}
void ImGui_Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}