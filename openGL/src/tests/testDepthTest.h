#ifndef TEST_DEPTH_TEST
#define TEST_DEPTH_TEST

#include "test.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

#include <vector>
#include <memory>
#include <algorithm>


#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

class TestAdvancedGL : public Test
{
public:
	TestAdvancedGL(Camera&, GLFWwindow*);
	~TestAdvancedGL();

	void OnUpdate() override;
	void OnImGuiRender() override;
private:
	std::unique_ptr<VertexArray> CubeVA;
	std::unique_ptr<VertexBuffer> CubeVB;
	std::unique_ptr<VertexArray> FloorVA;
	std::unique_ptr<VertexBuffer> FloorVB;
	std::unique_ptr<VertexArray> TransVA;
	std::unique_ptr<VertexBuffer> TransVB;

	std::unique_ptr<Texture> CrateTex;
	std::unique_ptr<Texture> FloorTex;
	std::unique_ptr<Texture> GrassTex;

	std::unique_ptr<Shader> shader;
	std::unique_ptr<Shader> stencilShader;

	std::vector<glm::vec3> transPos;
	void sortDist();

	Camera& camera;
	GLFWwindow* window;

	int func_type;
	int old_func_type;

	bool visDepBuf;
	bool visLinDepBuf;
	bool useStencil;
	bool transRender;
};
#endif


