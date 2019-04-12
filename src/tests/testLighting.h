#ifndef TEST_LIGHTING_H
#define TEST_LIGHTING_H

#include "test.h"

#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Texture.h"

#include <memory>
#include <string>

#include "imgui-master/imgui.h"

class TestLighting : public Test
{
public:
	TestLighting(Camera&, GLFWwindow*);
	~TestLighting();

	void OnUpdate() override;
	void key_callback(int key, int action) override;
	void OnImGuiRender() override;
private:
	GLFWwindow *m_window;
	Camera &m_camera;

	std::unique_ptr<VertexBuffer> m_cubeBuf;
	std::unique_ptr<VertexArray> m_cubeVA;
	std::unique_ptr<VertexArray> m_lampVA;
	std::unique_ptr<Shader> m_lightingShader;
	std::unique_ptr<Shader> m_lampShader;
	std::unique_ptr<Texture> m_crateTex;

	float m_theta;
	bool m_isRotating;
	bool m_isSpotLighton;

	std::unique_ptr<Texture> m_diffMap;
	std::unique_ptr<Texture> m_specMap;
	std::unique_ptr<Texture> m_emisMap;

	glm::vec3 *m_cubePos;
	glm::vec3 *m_pointLightPositions;
	glm::vec3 m_dirLightColor;
};

#endif 
