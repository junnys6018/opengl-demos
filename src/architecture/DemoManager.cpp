#include "DemoManager.h"
#include <iostream>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "FirstPersonCamera.h"
#include "OrbitCamera.h"

#include "debug.h"

DemoManager::DemoManager()
	:m_CurrentDemo(nullptr), m_ShowControlsWindow(false), m_ShowTestWindow(true), m_ShowPosition(true), m_Window(nullptr)
{
	m_camera = new FirstPersonCamera();
	RegisterTests();
}

void DemoManager::Initialize(GLFWwindow* win)
{
	m_Window = win;
}

DemoManager::~DemoManager()
{
	delete m_camera;
	delete m_CurrentDemo;
	for (auto& test : m_Demos)
	{
		delete test;
	}
}

void DemoManager::RegisterTests()
{
	m_Demos.push_back(new TestTriangleDeployer());
	m_Demos.push_back(new TestPlanetsDeployer());
	m_Demos.push_back(new TestLightingDeployer());
	m_Demos.push_back(new TestAdvancedGLDeployer());
	m_Demos.push_back(new TestFrameBufDeployer());
	m_Demos.push_back(new TestCubeMapDeployer());
	m_Demos.push_back(new TestAdvGLSLDeployer());
	m_Demos.push_back(new TestInstancingDeployer());
	m_Demos.push_back(new TestAdvLightDeployer());
	m_Demos.push_back(new TestShadowsDeployer());
	m_Demos.push_back(new TestPointShadowsDeployer());
	m_Demos.push_back(new TestNormMapDeployer());
	m_Demos.push_back(new TestParaMapDeployer());
	m_Demos.push_back(new TestBloomDeployer());
	m_Demos.push_back(new TestDeferredDeployer());
	m_Demos.push_back(new TestSSAODeployer());
	m_Demos.push_back(new TestDirectPBRDeployer());
	m_Demos.push_back(new TestIBL_PBRDeployer());
}

void DemoManager::OnImGuiRender(unsigned int fps)
{
	if (m_ShowControlsWindow)
	{
		ImGui::Begin("Controls", &m_ShowControlsWindow);
		ImGui::Text("ESC - Toggle Camera");
		ImGui::Text("SCROLL - Change FOV");
		ImGui::Text("WASD - Move");
		ImGui::Text("F - Toggle Fullscreen");
		ImGui::End();
	}

	if (m_ShowTestWindow)
	{
		ImGui::Begin("Demo Menu", &m_ShowTestWindow);
		if (m_CurrentDemo)
		{
			if (ImGui::Button("<-"))
			{
				delete m_CurrentDemo;
				m_CurrentDemo = nullptr;
			}
			else
				m_CurrentDemo->OnImGuiRender();
		}
		else
		{
			ImGui::RadioButton("Walk Cam", &m_ActiveCamera, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Circle Cam", &m_ActiveCamera, 1);
			ImGui::Separator();
			if (m_ActiveCamera != m_OldActiveCamera)
			{
				m_OldActiveCamera = m_ActiveCamera;
				delete m_camera;
				if (m_ActiveCamera == 0)
					m_camera = new FirstPersonCamera();
				else if (m_ActiveCamera == 1)
					m_camera = new OrbitCamera();
			}
			ImGui::Text("Tests:");

			for (int i = 0; i < m_Demos.size(); i++)
			{
				if (i % 2 == 1)
					ImGui::SameLine(0.0f, 20.0f);
				if (ImGui::Button(m_Demos[i]->getName(), ImVec2(120.0f, 25.0f)))
				{
					ImGui::OpenPopup(m_Demos[i]->getName());
				}
			}

			for (int i = 0; i < m_Demos.size(); ++i)
			{
				if (ImGui::BeginPopup(m_Demos[i]->getName()))
				{
					if (m_Demos[i]->OnImguiUpdate())
					{
						m_CurrentDemo = m_Demos[i]->Deploy(m_camera, m_Window);
					}
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();
	}

	// FPS counter
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 10.0f, viewport->Pos.y + 10.0f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
		ImGui::Begin("My Overlay", NULL, ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

		ImGui::Text("%.3f ms/frame (%i FPS)", 1000.0f / fps, fps);
		if (m_ShowPosition)
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
			ImGui::Checkbox("Test Menu", &m_ShowTestWindow);
			ImGui::Checkbox("Controls Window", &m_ShowControlsWindow);
			ImGui::Checkbox("Show Camera Pos", &m_ShowPosition);
			ImGui::End();
		}
		ImGui::End();
	}
}

void DemoManager::GameLoop()
{
	// Timing
	double previousTime = glfwGetTime();
	unsigned int frameCount = 0;
	unsigned int fps = 0;

	while (!glfwWindowShouldClose(m_Window))
	{
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - previousTime >= 1.0) // 1 second
		{
			fps = frameCount;
			frameCount = 0;
			previousTime = currentTime;
		}
		if (m_CurrentDemo)
			m_CurrentDemo->OnUpdate();
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
		glfwMakeContextCurrent(m_Window);

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}