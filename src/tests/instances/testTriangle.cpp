#include "testTriangle.h"
#include "debug.h"

TestTriangle::TestTriangle()
	:m_clearColor(0.2f, 0.3f, 0.3f, 1.0f)
{
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glBindVertexArray(VAO));

	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

	float tris[] = 
	{
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
	};
	
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(tris), tris, GL_STATIC_DRAW));

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	m_shader = std::make_unique<Shader>("res/Shaders/Triangle.shader");
	m_shader->Use();

	GLCall(glDisable(GL_DEPTH_TEST));
}
TestTriangle::~TestTriangle()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void TestTriangle::OnUpdate()
{			
	glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 3););
}			
void TestTriangle::OnImGuiRender()
{
	ImGui::ColorEdit3("Clear Color", (float*)&m_clearColor);
	
}