#include "test.h"
#include <iostream>

#include "imgui-master/imgui.h"
#include "imgui-master/imgui_impl_glfw.h"
#include "imgui-master/imgui_impl_opengl3.h"
TestManager::TestManager()
	:m_currentTest(nullptr), show_demo_window(false)
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

void TestManager::OnImGuiRender()
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
		ImGui::Checkbox("Demo Window", &show_demo_window);
		for (auto &e : m_tests)
			if (ImGui::Button(e.first.c_str()))
			{
				m_currentTest = e.second();
			}
	}
	ImGui::End();
}

