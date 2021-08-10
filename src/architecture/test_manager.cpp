#include "test_manager.h"
#include <iostream>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "Walk_Camera.h"
#include "Static_Camera.h"

#include "debug.h"

TestManager::TestManager()
	:m_currentTest(nullptr), show_controls_window(false), show_test_window(true), show_pos(true), m_window(nullptr)
{
	m_camera = new Walk_Camera();
	registerTests();
}

void TestManager::init(GLFWwindow* win)
{
	m_window = win;
}

TestManager::~TestManager()
{
	delete m_camera;
	delete m_currentTest;
	for (auto& test : m_tests)
	{
		delete test;
	}
}

void TestManager::registerTests()
{
	m_tests.push_back(new TestTriangleDeployer());
	m_tests.push_back(new TestPlanetsDeployer());
	m_tests.push_back(new TestLightingDeployer());
	m_tests.push_back(new TestAdvancedGLDeployer());
	m_tests.push_back(new TestFrameBufDeployer());
	m_tests.push_back(new TestCubeMapDeployer());
	m_tests.push_back(new TestAdvGLSLDeployer());
	m_tests.push_back(new TestInstancingDeployer());
	m_tests.push_back(new TestAdvLightDeployer());
	m_tests.push_back(new TestShadowsDeployer());
	m_tests.push_back(new TestPointShadowsDeployer());
	m_tests.push_back(new TestNormMapDeployer());
	m_tests.push_back(new TestParaMapDeployer());
	m_tests.push_back(new TestBloomDeployer());
	m_tests.push_back(new TestDeferredDeployer());
	m_tests.push_back(new TestSSAODeployer());

	m_tests.push_back(new TestDirectPBRDeployer());
	m_tests.push_back(new TestIBL_PBRDeployer());
}

void TestManager::OnImGuiRender(unsigned int fps)
{
	if (show_controls_window)
	{
		ImGui::Begin("Controls", &show_controls_window);
		ImGui::Text("ESC - Toggle Camera");
		ImGui::Text("SCROLL - Change FOV");
		ImGui::Text("WASD - Move");
		ImGui::Text("F - Toggle Fullscreen");
		ImGui::End();
	}

	if (show_test_window)
	{
		ImGui::Begin("Test Menu", &show_test_window);
		if (m_currentTest)
		{
			if (ImGui::Button("<-"))
			{
				delete m_currentTest;
				m_currentTest = nullptr;
			}
			else
				m_currentTest->OnImGuiRender();
		}
		else
		{
			ImGui::RadioButton("Walk Cam", &active_camera, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Circle Cam", &active_camera, 1);
			ImGui::Separator();
			if (active_camera != old_active_camera)
			{
				old_active_camera = active_camera;
				delete m_camera;
				if (active_camera == 0)
					m_camera = new Walk_Camera();
				else if (active_camera == 1)
					m_camera = new Static_Camera();
			}
			ImGui::Text("Tests:");

			for (int i = 0; i < m_tests.size(); i++)
			{
				if (i % 2 == 1)
					ImGui::SameLine(0.0f, 20.0f);
				if (ImGui::Button(m_tests[i]->getName(), ImVec2(120.0f, 25.0f)))
				{
					ImGui::OpenPopup(m_tests[i]->getName());
				}
			}

			for (int i = 0; i < m_tests.size(); ++i)
			{
				if (ImGui::BeginPopup(m_tests[i]->getName()))
				{
					if (m_tests[i]->OnImguiUpdate())
					{
						m_currentTest = m_tests[i]->Deploy(m_camera, m_window);
					}
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();
	}

	// FPS counter
	{
		// FIXME-VIEWPORT: Select a default viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 10.0f, viewport->Pos.y + 10.0f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
		ImGui::Begin("My Overlay", NULL, ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

		ImGui::Text("%.3f ms/frame (%i FPS)", 1000.0f / fps, fps);
		if (show_pos)
		{
			ImGui::Text("CamPos: x: %.1f y: %.1f z: %.1f", m_camera->getCameraPos().x, m_camera->getCameraPos().y, m_camera->getCameraPos().z);
			glm::vec3 dir = m_camera->getCameraDir();
			float yaw = glm::degrees(glm::atan(dir.x, dir.z));
			float pitch = glm::degrees(glm::asin(dir.y));

			const char* facing = "";
			if (yaw >= -45.0f && yaw < 45.0f)
				facing = "+z";
			else if (yaw >= 45.0f && yaw < 135.0f)
				facing = "+x";
			else if (yaw >= 135.0f || yaw < -135.0f)
				facing = "-z";
			else
				facing = "-x";

			ImGui::Text("facing: %s yaw: %.0f pitch: %.0f", facing, yaw, pitch);
		}
		if (ImGui::BeginPopupContextWindow("item context menu"))
		{
			ImGui::Checkbox("Test Menu", &show_test_window);
			ImGui::Checkbox("Controls Window", &show_controls_window);
			ImGui::Checkbox("Show Camera Pos", &show_pos);
			ImGui::End();
		}
		ImGui::End();
	}
}

void TestManager::gameLoop()
{
	// Timing
	double previousTime = glfwGetTime();
	unsigned int frameCount = 0;
	unsigned int fps = 0;

	while (!glfwWindowShouldClose(m_window))
	{
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - previousTime >= 1.0) // 1 second
		{
			fps = frameCount;
			frameCount = 0;
			previousTime = currentTime;
		}
		if (m_currentTest)
			m_currentTest->OnUpdate();
		else
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		OnImGuiRender(fps);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(m_window);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}