#pragma once

#define PI_F			3.14159265358979f
#define WINDOW_NAME		"Genesis"
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define NUM_DRAWS		50000
#define NUM_STARS		2000

enum MODE
{
	MODE_FIRST,
	MODE_MULTIDRAW = 0,
	MODE_SEPARATE_DRAWS,
	MODE_MAX = MODE_SEPARATE_DRAWS
};

struct DrawArraysIndirectCommand
{
	GLuint count;
	GLuint primCount;
	GLuint first;
	GLuint baseInstance;
};
