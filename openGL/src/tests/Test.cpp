#include "test.h"
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Walk_Camera.h"
#include "Static_Camera.h"

#include "__Test__.h"
#include "debug.h"

#define NAME 0
#define INIT 1
#define VARS 2
static uint16_t instances = 10000; // TEST_INSTANCING

static uint16_t blur_scale = 4; // TEST_BLOOM
static uint16_t nr_passes = 8;

static uint16_t nr_lights = 32; // TEST_DEFERRED

static std::string hdrPath = "res/Textures/RadianceMap/Alexs_Apt_2k.hdr"; // TEST-IBL
TestManager::TestManager(GLFWwindow** win)
	:m_currentTest(nullptr), show_controls_window(false), show_test_window(true), show_pos(true), m_window(win)
{
	m_camera = new Walk_Camera();
}
TestManager::~TestManager()
{

}

void TestManager::parseInput(std::string line)
{
	std::istringstream s(line);
	std::string token;
	s >> token;
	if (token == "/test")
	{
		bool success = false;
		s >> token;
		for (auto test : m_tests)
			if (std::get<NAME>(test) == token)
			{
				delete m_currentTest;
				m_currentTest = std::get<INIT>(test)(m_camera, *m_window);
				success = true;
				break;
			}
		if (!success)
			std::cout << token << " is an invalid test!\n";
	}
}

void TestManager::registerTest(std::string name, std::function<Test* (Base_Camera*, GLFWwindow*)> initalizer, std::function<bool()> varInit)
{
	m_tests.push_back(std::make_tuple(name, initalizer, varInit));
}

void TestManager::registerTests()
{
	auto noInit = []()->bool {return true; };
	registerTest("triangle", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestTriangle(); }, noInit);
	registerTest("planets", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestPlanets(cam, win); }, noInit);
	registerTest("lighting", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestLighting(cam, win); }, noInit);
	registerTest("advOpenGL", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestAdvancedGL(cam, win); }, noInit);
	registerTest("frameBuf", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestFrameBuf(cam, win); }, noInit);
	registerTest("cubeMap", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestCubeMap(cam, win); }, noInit);
	registerTest("advGLSL", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestAdvGLSL(cam, win); }, noInit);
	registerTest("Instancing", [](Base_Camera * cam, GLFWwindow * win)->Test * { return new TestInstancing(cam, win, instances); },
		[]()->bool {
			ImGui::Text("#instances:");
			ImGui::PushItemWidth(-1);
			ImGui::InputScalar("##Value", ImGuiDataType_U16, &instances);
			ImGui::PopItemWidth();
			return ImGui::Button("Enter");
		});
	registerTest("advLight", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestAdvLight(cam, win); }, noInit);
	registerTest("Shadows", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestShadows(cam, win); }, noInit);
	registerTest("Point-Shadow", [](Base_Camera * cam, GLFWwindow * win) ->Test * {return new TestPointShadows(cam, win); }, noInit);
	registerTest("NormMap", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestNormMap(cam, win); }, noInit);
	registerTest("ParaMap", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestParaMap(cam, win); }, noInit);
	registerTest("Bloom", [](Base_Camera * cam, GLFWwindow * win)->Test * { return new TestBloom(cam, win, blur_scale, nr_passes); },
		[]()->bool {
			ImGui::PushItemWidth(-1);
			ImGui::Text("Blur Scale:");
			ImGui::InputScalar("##Value0", ImGuiDataType_U16, &blur_scale);
			ImGui::Text("# of Blur passes");
			ImGui::InputScalar("##Value1", ImGuiDataType_U16, &nr_passes);
			ImGui::PopItemWidth();
			return ImGui::Button("Enter");
		});
	registerTest("Deferred", [](Base_Camera * cam, GLFWwindow * win)->Test * { return new TestDeferred(cam, win, nr_lights); },
			[]()->bool {
			ImGui::Text("# Lights");
			ImGui::PushItemWidth(-1);
			ImGui::InputScalar("##Value", ImGuiDataType_U16, &nr_lights);
			ImGui::PopItemWidth();
			return ImGui::Button("Enter");
		});
	registerTest("SSAO", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestSSAO(cam, win); }, noInit);
	registerTest("Direct-PBR", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestDirectPBR(cam, win); }, noInit);
	registerTest("IBL-PBR", [](Base_Camera * cam, GLFWwindow * win)->Test * {return new TestIBL_PBR(cam, win, hdrPath); },
		[]()->bool {
			ImGui::PushItemWidth(-1);
			ImGui::Text("Skybox:");
			if (ImGui::Button("Alexs Apt"))
			{
				hdrPath = "res/Textures/RadianceMap/Alexs_Apt_2k.hdr";
				return true;
			}
			if (ImGui::Button("Circus Stage"))
			{
				hdrPath = "res/Textures/RadianceMap/Circus_Backstage_3k.hdr";
				return true;
			}
			if (ImGui::Button("Canyon"))
			{
				hdrPath = "res/Textures/RadianceMap/GCanyon_C_YumaPoint_3k.hdr";
				return true;
			}
			ImGui::PopItemWidth();
			return false;
		});
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
			const int maxButtonCol = 8;
			for (int i = 0; i < maxButtonCol; ++i)
			{
				unsigned int index = i;
				while (true)
				{
					if (ImGui::Button(std::get<NAME>(m_tests[index]).c_str(), ImVec2(120.0f, 25.0f)))
					{
						ImGui::OpenPopup(std::get<NAME>(m_tests[index]).c_str());
					}
					index += maxButtonCol;
					if (index < 16) // first 16 tests are non pbr
						ImGui::SameLine(0.0f, 20.0f);
					else break;
				}
			}
			// PBR tests
			ImGui::Separator();
			ImGui::Text("Physically Based Rendering");
			for (int i = 16; i != m_tests.size(); i++)
			{
				if (ImGui::Button(std::get<NAME>(m_tests[i]).c_str(), ImVec2(120.0f, 25.0f)))
				{
					ImGui::OpenPopup(std::get<NAME>(m_tests[i]).c_str());
				}
					ImGui::SameLine(0.0f, 20.0f);
			}

			for (int i = 0; i < m_tests.size(); ++i)
			{
				if (ImGui::BeginPopup(std::get<NAME>(m_tests[i]).c_str()))
				{
					if (std::get<VARS>(m_tests[i])())
					{
						m_currentTest = std::get<INIT>(m_tests[i])(m_camera, *m_window);
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

	while (!glfwWindowShouldClose(*m_window))
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
		glfwMakeContextCurrent(*m_window);

		glfwSwapBuffers(*m_window);
		glfwPollEvents();
	}
}


