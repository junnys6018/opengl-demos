#include "ScreenShot.h"
#include <glad/glad.h>
#include "stb_image_write.h"
#include "debug.h"

#include <iostream>
#include <ctime>

#pragma warning(disable : 4996)

bool take_screenshot(int width, int height)
{
	void* data = new char[width * height * 4];
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data));
	stbi_flip_vertically_on_write(1);

	std::time_t time = std::time(0);
	std::tm* date = std::localtime(&time);
	char date_string[50];
	std::sprintf(date_string, "%i-%i-%i_%i-%.2i-%.2i_sceenshot.png", date->tm_year + 1900, date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);

	bool success = stbi_write_png(date_string, width, height, 3, data, width * 3) == 1;
	if (success)
		std::cout << "Took Screenshot\n";
	else
		std::cout << "Failed to take screenshot\n";
	delete[] data;
	return success;
}
