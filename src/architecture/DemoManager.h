#pragma once

#include "demos/test.h"
#include "demos/testPlanets.h"
#include "demos/testLighting.h"
#include "demos/testTriangle.h"
#include "demos/testDepthTest.h"
#include "demos/testFrameBuf.h"
#include "demos/testCubeMap.h"
#include "demos/testAdvGLSL.h"
#include "demos/testInstancing.h"
#include "demos/testAdvLight.h"
#include "demos/testShadows.h"
#include "demos/testPointShadow.h"
#include "demos/testNormalMap.h"
#include "demos/testParallaxMap.h"
#include "demos/testBloom.h"
#include "demos/testDeferred.h"
#include "demos/testSSAO.h"
#include "demos/PBR/testDirectPBR.h"
#include "demos/PBR/testIBL_PBR.h"

#include "GLFW/glfw3.h"

class DemoManager
{
public:
	DemoManager();
	void Initialize(GLFWwindow* win);

	~DemoManager();

	void GameLoop();
	Demo* m_CurrentDemo;
	AbstractCamera* m_camera;
private:
	void RegisterTests();
	void OnImGuiRender(unsigned int fps);

	std::vector<DemoDeployer*> m_Demos;

	// Imgui members
	bool m_ShowControlsWindow;
	bool m_ShowTestWindow;
	bool m_ShowPosition;
	int m_ActiveCamera, m_OldActiveCamera;

	GLFWwindow* m_Window;
};
