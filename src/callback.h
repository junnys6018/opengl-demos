#ifndef CALLBACK_H
#define CALLBACK_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
	camera.mouse_callback(xpos, ypos);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_3 && action == GLFW_RELEASE)
		isRotating = !isRotating;
	camera.key_callback(window, key, action);
}
#endif
