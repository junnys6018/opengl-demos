#ifndef TEST_IBL_PBR
#define TEST_IBL_PBR
#include "../test.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"
#include "Timer.h"

#include <memory>
#include "stb_image.h"

class TestIBL_PBR : public Test
{
public:
	TestIBL_PBR(Base_Camera* cam, GLFWwindow* win, const std::string& hdrPath);
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
	Base_Camera* m_camera;
	GLFWwindow* m_window;
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
#endif