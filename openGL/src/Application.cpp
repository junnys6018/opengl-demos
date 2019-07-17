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
TestManager test_mgr;
extern uint16_t instances;

#include "callback.h"
int main(int argc, char* argv[])
{

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return -1;
	GLFWwindow* window = glfwCreateWindow(1080, 720, "OpenGL tests", NULL, NULL);
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

	test_mgr.registerTest("triangle"    , [ ]()->Test* {return new TestTriangle(); }                            );
	test_mgr.registerTest("planets"     , [&]()->Test* {return new TestPlanets(camera, window); }               );
	test_mgr.registerTest("lighting"    , [&]()->Test* {return new TestLighting(camera, window); }              );
	test_mgr.registerTest("advOpenGL"   , [&]()->Test* {return new TestAdvancedGL(camera, window); }            );
	test_mgr.registerTest("frameBuf"    , [&]()->Test* {return new TestFrameBuf(camera, window); }              );
	test_mgr.registerTest("cubeMap"     , [&]()->Test* {return new TestCubeMap(camera, window); }               );
	test_mgr.registerTest("advGLSL"     , [&]()->Test* {return new TestAdvGLSL(camera, window); }               );
	test_mgr.registerTest("Instancing"  , [&]()->Test* {return new TestInstancing(camera, window, instances); } );
	test_mgr.registerTest("advLight"    , [&]()->Test* {return new TestAdvLight(camera, window); }              );
	test_mgr.registerTest("Shadows"     , [&]()->Test* {return new TestShadows(camera, window); }               );
	test_mgr.registerTest("NormMap"     , [&]()->Test* {return new TestNormMap(camera, window); }               );
	test_mgr.registerTest("ParaMap"     , [&]()->Test* {return new TestParaMap(camera, window); }               );
	test_mgr.registerTest("Bloom"       , [&]()->Test* {return new TestBloom(camera, window); }                 );
	test_mgr.registerTest("Deferred"    , [&]()->Test* {return new TestDeferred(camera, window); }              );
	test_mgr.registerTest("SSAO"        , [&]()->Test* {return new TestSSAO(camera, window); }                  );

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

	// Timing
	double previousTime = glfwGetTime();
	unsigned int frameCount = 0;
	unsigned int fps = 0;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - previousTime >= 1.0) // 1 second
		{
			fps = frameCount;
			frameCount = 0;
			previousTime = currentTime;
		}
		if (test_mgr.m_currentTest)
			test_mgr.m_currentTest->OnUpdate();
		else
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f)); 
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		test_mgr.OnImGuiRender(fps, camera.getCameraPos().x, camera.getCameraPos().y, camera.getCameraPos().z, camera.InUse());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}