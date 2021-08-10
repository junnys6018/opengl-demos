#ifndef TIMER_H
#define TIMER_H
#include <glad/glad.h>

// Times OpenGL calls
class Timer
{
private:
	unsigned int queryID[2];
	unsigned int queryBackBuffer, queryFrontBuffer;
	GLuint64 elapsedTime; // in ns
	void swapBuffers();
public:
	Timer();
	~Timer();
	void begin();
	void end();
	GLuint64 getTime();

};

#endif // !TIMER_H
