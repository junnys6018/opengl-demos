#ifndef TEST_IBL_PBR
#define TEST_IBL_PBR
#include "../test.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"
#include "Timer.h"

#include <memory>
#include "stb_image.h"

class TestIBL_PBR : public Demo
{
public:
	TestIBL_PBR(AbstractCamera* cam, GLFWwindow* win, const std::string& hdrPath);
	~TestIBL_PBR();

	void OnUpdate() override;
	void OnImGuiRender() override;
	void framebuffer_size_callback(int width, int height) override;
private:
	void generateTextures(const std::string& path, int index);
	void bindTextures(int index);

	void allocate_mem_to_cube_map(unsigned int* cubemap, int width);
	void render_to_cube_map(unsigned int* cubemap, int width, std::unique_ptr<Shader>& shader, int mip = 0);
private:
	AbstractCamera* m_camera;
	GLFWwindow* m_Window;
	Timer timer;

	int sWidth, sHeight;
	float metalness, roughness, mipLevel;
	glm::vec3 albedo;
	unsigned int renderFlags;
	int renderMode, oldRenderMode, skyboxTarget, oldSkyboxTarget;

	unsigned int hdrTexture, EnviromentMap, IrradianceMap, CaptureFBO, CaptureRBO;
	unsigned int PrefilterMap;
	const int FBOwidth = 1024;

	std::unique_ptr<Object> o_Sphere;
	std::unique_ptr<Object> o_Cylinder;
	std::unique_ptr<Object> o_Cerberus;
	std::unique_ptr<Object> o_Cube;

	std::unique_ptr<Shader> s_Shader;
	std::unique_ptr<Shader> s_SkyBox;

	// Preprocessing shaders
	std::unique_ptr<Shader> s_Equ2Cube;
	std::unique_ptr<Shader> s_Convolution;
	std::unique_ptr<Shader> s_PreFilter;

	std::unique_ptr<Texture> t_Albedo[2];
	std::unique_ptr<Texture> t_Normal[2];
	std::unique_ptr<Texture> t_Metalness[2];
	std::unique_ptr<Texture> t_Roughness[2];
	std::unique_ptr<Texture> t_brdfLUT;
};

class TestIBL_PBRDeployer : public DemoDeployer
{
public:
	TestIBL_PBRDeployer()
		: DemoDeployer("IBL-PBR"), m_hdrPath("res/Textures/RadianceMap/Alexs_Apt_2k.hdr")
	{

	}

	Demo* Deploy(AbstractCamera* cam, GLFWwindow* win) override
	{
		return new TestIBL_PBR(cam, win, m_hdrPath);
	}

	bool OnImguiUpdate() override
	{
		ImGui::PushItemWidth(-1);
		ImGui::Text("Skybox:");
		if (ImGui::Button("Alexs Apt"))
		{
			m_hdrPath = "res/Textures/RadianceMap/Alexs_Apt_2k.hdr";
			return true;
		}
		if (ImGui::Button("Circus Stage"))
		{
			m_hdrPath = "res/Textures/RadianceMap/Circus_Backstage_3k.hdr";
			return true;
		}
		if (ImGui::Button("Canyon"))
		{
			m_hdrPath = "res/Textures/RadianceMap/GCanyon_C_YumaPoint_3k.hdr";
			return true;
		}
		ImGui::PopItemWidth();
		return false;
	}

private:
	std::string m_hdrPath;
};

#endif
