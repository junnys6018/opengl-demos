#ifndef SCREENSHOT_H
#define SCREENSHOT_H

/*
	Notes:
		width and height specify the dimensions of the screenbuffer,
		this means the resolution of the screenshot is fixed to the 
		screenbuffer size
*/
bool take_screenshot(int width, int height);

#endif
