#pragma once
#include <GL/glew.h>

#define PI_F			3.14159265358979f
#define WINDOW_NAME		"Genesis"
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define NUM_DRAWS		50000
#define NUM_STARS		2000

static unsigned int seed = 0x13371337;

struct DrawArraysIndirectCommand
{
	GLuint count;
	GLuint primCount;
	GLuint first;
	GLuint baseInstance;
};
