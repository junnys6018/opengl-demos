#include "test.h"
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "__Test__.h"
#include "debug.h"

#define NAME 0
#define INIT 1
#define VARS 2
static uint16_t instances = 10000; // TEST_INSTANCING

static uint16_t blur_scale = 4; // TEST_BLOOM
static uint16_t nr_passes = 8;

static uint16_t nr_lights = 32; // TEST_DEFERRED
TestManager::TestManager(Camera& cam, GLFWwindow** win)
	:m_currentTest(nullptr), show_demo_window(false), show_overlay(true), m_camera(cam), m_window(win)
{
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

void TestManager::registerTest(std::string name, std::function<Test* (Camera&, GLFWwindow*)> initalizer, std::function<bool()> varInit)
{
	m_tests.push_back(std::make_tuple(name, initalizer, varInit));
}

void TestManager::registerTests()
{
	auto noInit = []()->bool {return true; };
	registerTest("triangle", [](Camera & cam, GLFWwindow * win)->Test * {return new TestTriangle(); }, noInit);
	registerTest("planets", [](Camera & cam, GLFWwindow * win)->Test * {return new TestPlanets(cam, win); }, noInit);
	registerTest("lighting", [](Camera & cam, GLFWwindow * win)->Test * {return new TestLighting(cam, win); }, noInit);
	registerTest("advOpenGL", [](Camera & cam, GLFWwindow * win)->Test * {return new TestAdvancedGL(cam, win); }, noInit);
	registerTest("frameBuf", [](Camera & cam, GLFWwindow * win)->Test * {return new TestFrameBuf(cam, win); }, noInit);
	registerTest("cubeMap", [](Camera & cam, GLFWwindow * win)->Test * {return new TestCubeMap(cam, win); }, noInit);
	registerTest("advGLSL", [](Camera & cam, GLFWwindow * win)->Test * {return new TestAdvGLSL(cam, win); }, noInit);
	registerTest("Instancing", [](Camera & cam, GLFWwindow * win)->Test * { return new TestInstancing(cam, win, instances); },
		[]()->bool {
		ImGui::Text("#instances:");
		ImGui::PushItemWidth(-1);
		ImGui::InputScalar("##Value", ImGuiDataType_U16, &instances);
		ImGui::PopItemWidth();
		return ImGui::Button("Enter");
	});
	registerTest("advLight", [](Camera & cam, GLFWwindow * win)->Test * {return new TestAdvLight(cam, win); }, noInit);
	registerTest("Shadows", [](Camera & cam, GLFWwindow * win)->Test * {return new TestShadows(cam, win); }, noInit);
	registerTest("Point-Shadow", [](Camera & cam, GLFWwindow * win) ->Test * {return new TestPointShadows(cam, win); }, noInit);
	registerTest("NormMap", [](Camera & cam, GLFWwindow * win)->Test * {return new TestNormMap(cam, win); }, noInit);
	registerTest("ParaMap", [](Camera & cam, GLFWwindow * win)->Test * {return new TestParaMap(cam, win); }, noInit);
	registerTest("Bloom", [](Camera & cam, GLFWwindow * win)->Test * { return new TestBloom(cam, win, blur_scale, nr_passes); },
		[]()->bool {
		ImGui::PushItemWidth(-1);
		ImGui::Text("Blur Scale:");
		ImGui::InputScalar("##Value0", ImGuiDataType_U16, &blur_scale);
		ImGui::Text("# of Blur passes");
		ImGui::InputScalar("##Value1", ImGuiDataType_U16, &nr_passes);
		ImGui::PopItemWidth();
		return ImGui::Button("Enter");
	});
	registerTest("Deferred", [](Camera & cam, GLFWwindow * win)->Test * { return new TestDeferred(cam, win, nr_lights); },
		[]()->bool {
		ImGui::Text("# Lights");
		ImGui::PushItemWidth(-1);
		ImGui::InputScalar("##Value", ImGuiDataType_U16, &nr_lights);
		ImGui::PopItemWidth();
		return ImGui::Button("Enter");
	});
	registerTest("SSAO", [](Camera & cam, GLFWwindow * win)->Test * {return new TestSSAO(cam, win); }, noInit);
}

void TestManager::OnImGuiRender(unsigned int fps)
{
	if (!m_camera.InUse())
	{
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Begin("Test Menu");
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
			const int maxButtonCol = 8;
			ImGui::Checkbox("Demo Window", &show_demo_window);
			ImGui::SameLine();
			ImGui::Checkbox("Overlay", &show_overlay);
			ImGui::Separator();
			ImGui::Text("Tests:");
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
					if (index < m_tests.size())
						ImGui::SameLine(0.0f, 20.0f);
					else break;
				}
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
	if (show_overlay)
	{
		ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
		ImGui::Begin("Example: Simple overlay", NULL, ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

		ImGui::Text("%.3f ms/frame (%i FPS)", 1000.0f / fps, fps);
		if (ImGui::BeginPopupContextWindow("item context menu"))
		{
			ImGui::Checkbox("Overlay", &show_overlay);
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

		glfwSwapBuffers(*m_window);
		glfwPollEvents();
	}
}


