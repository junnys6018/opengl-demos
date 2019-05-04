#ifndef CALLBACK_H
#define CALLBACK_H
void error_callback(int error, const char* description)
{
	std::cout << "[GLFW ERROR] " << error << " DESCRIPTION: " << description << std::endl;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if (test_mgr.m_currentTest)
		test_mgr.m_currentTest->framebuffer_size_callback(width, height);
}
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.mouse_callback(xpos, ypos);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (test_mgr.m_currentTest)
		test_mgr.m_currentTest->key_callback(key, action);
	camera.key_callback(window, key, action);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (camera.InUse())
	{
		if (camera.m_FOV >= 1.0f && camera.m_FOV <= 90.0f)
			camera.m_FOV -= yoffset;
		if (camera.m_FOV <= 1.0f)
			camera.m_FOV = 1.0f;
		if (camera.m_FOV >= 90.0f)
			camera.m_FOV = 90.0f;

	}

}
void printBasicInfo()
{
	std::cout << "Status: Using GLEW " <<  glewGetString(GLEW_VERSION)  << std::endl;
	std::cout << "OpenGl version: "    <<  glGetString(GL_VERSION)      << std::endl;
	std::cout << "GLFW version: "      <<  glfwGetVersionString()       << std::endl;
	int numAttributes;
	GLCall(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes));
	std::cout << "Maximum # of vertex attributes supported: " << numAttributes << '\n';
}
#endif