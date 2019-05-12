#include "test.h"
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "testInstancing.h"

extern uint16_t instances = 10000;

TestManager::TestManager()
	:m_currentTest(nullptr), show_demo_window(false), show_pos(false), show_overlay(true)
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
			if (test.first == token)
			{
				delete m_currentTest;
				m_currentTest = test.second();
				success = true;
				break;
			}
		if (!success)
			std::cout << token << " is an invalid test!\n";
	}
}
void TestManager::registerTest(std::string name, std::function<Test*()> fp)
{
	m_tests.push_back(std::make_pair(name, fp));
}

void TestManager::OnImGuiRender(unsigned int fps, float posX, float posY, float posZ, bool camInUse)
{
	if (!camInUse)
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
					// hack for testInstancing TODO: refactor test_mgr
					if (ImGui::Button(m_tests[index].first.c_str(), ImVec2(120.0f, 25.0f)))
					{
						if (m_tests[index].first != "Instancing")
							m_currentTest = m_tests[index].second();
						else
							ImGui::OpenPopup("config");
					}
					index += maxButtonCol;
					if (index < m_tests.size())
						ImGui::SameLine(0.0f, 20.0f);
					else break;
				}
			}
			if (ImGui::BeginPopup("config"))
			{
				ImGui::Text("#instances:");
				ImGui::PushItemWidth(-1);
				ImGui::InputScalar("##Value", ImGuiDataType_U16, &instances);
				ImGui::PopItemWidth();
				if (ImGui::Button("Enter"))
				{
					ImGui::EndPopup();
					for (auto e : m_tests)
					{
						if (e.first == "Instancing")
							m_currentTest = e.second();
					}
				}
				else
					ImGui::EndPopup();
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
			ImGui::Checkbox("Show Pos", &show_pos);
			ImGui::Checkbox("Overlay", &show_overlay);
			ImGui::End();
		}
		if (show_pos)
		{
			ImGui::Text("x: %.3f y: %.3f z: %.3f", posX, posY, posZ);
		}
		ImGui::End();
	}
}

