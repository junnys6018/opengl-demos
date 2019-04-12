#define GLEW_BUILD
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>

#include "debug.h"
#include "Object.h"
#include "Camera.h"

#include "tests/testPlanets.h"
#include "tests/testLighting.h"
#include "tests/testTriangle.h"
#include "tests/testDepthTest.h"
#include "tests/testFrameBuf.h"

#include "imgui-master/imgui.h"
#include "imgui-master/imgui_impl_glfw.h"
#include "imgui-master/imgui_impl_opengl3.h"

Camera camera;
TestManager test_mgr;

// callback functions defined here
#include "callback.h"
int main(int argc, char* argv[])
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(1080, 720, "OpenGL tests", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW init error\n";

	printBasicInfo();

	// Set callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Pre render loop preperation
	glfwSwapInterval(1); // enable vsync

	test_mgr.registerTest("triangle"    , [ ]()->Test* {return new TestTriangle(); }                  );
	test_mgr.registerTest("planets"	    , [&]()->Test* {return new TestPlanets(camera, window); }     );
	test_mgr.registerTest("lighting"    , [&]()->Test* {return new TestLighting(camera, window); }    );
	test_mgr.registerTest("advOpenGL"   , [&]()->Test* {return new TestAdvancedGL(camera, window); }  );
	test_mgr.registerTest("frameBuf"    , [&]()->Test* {return new TestFrameBuf(camera, window); }    );

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
	io.Fonts->AddFontFromFileTTF("res/Fonts/Consola.ttf", 18);
	io.IniFilename = "Vendor/imgui-master/imgui.ini";

	//std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	//for (int i = 0; i != 50; ++i)
	//{
	//	Object obj("res/Objects/Pokemon/Blastoise/Blastoise.obj");
	//}
	//std::chrono::system_clock::time_point b = std::chrono::system_clock::now();
	//std::chrono::duration<double, std::milli> work_time = b - a;
	//std::cout << "Loaded in " << work_time.count() / 50.0 << "ms\n";

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (test_mgr.m_currentTest)
			test_mgr.m_currentTest->OnUpdate();
		else
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f)); 
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}
		
		if (!camera.InUse())
			test_mgr.OnImGuiRender();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}