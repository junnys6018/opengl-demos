#include "Timer.h"
#include "debug.h"

Timer::Timer()
	:queryFrontBuffer(0), queryBackBuffer(1)
{
	GLCall(glGenQueries(2, queryID));
	// dummy query to prevent OpenGL errors from popping out
	GLCall(glBeginQuery(GL_TIME_ELAPSED, queryID[queryFrontBuffer]));
	GLCall(glEndQuery(GL_TIME_ELAPSED));
}

Timer::~Timer()
{
	GLCall(glDeleteQueries(2, queryID));
}

void Timer::begin()
{
	GLCall(glBeginQuery(GL_TIME_ELAPSED, queryID[queryBackBuffer]));
}

void Timer::end()
{
	GLCall(glEndQuery(GL_TIME_ELAPSED));
	GLCall(glGetQueryObjectui64v(queryID[queryFrontBuffer], GL_QUERY_RESULT, &elapsedTime));
	swapBuffers();
}

GLuint64 Timer::getTime()
{
	return elapsedTime;
}

void Timer::swapBuffers()
{
	if (queryBackBuffer) 
	{
		queryBackBuffer = 0;
		queryFrontBuffer = 1;
	}
	else 
	{
		queryBackBuffer = 1;
		queryFrontBuffer = 0;
	}
}
