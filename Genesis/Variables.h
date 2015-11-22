#pragma once
#include "InputManager.h"

const float PI_F = 3.14159265358979f;

// Random number generator
static unsigned int seed = 0x13371337;

// Properties
extern GLuint screenWidth, screenHeight;
extern genesis::InputManager inputManager;

// Multi-Draw Indirect Enums and Struct
enum
{
	NUM_DRAWS = 50000
};
// Starfield Enum
enum
{
	NUM_STARS = 2000
};

struct DrawArraysIndirectCommand
{
	GLuint count;
	GLuint primCount;
	GLuint first;
	GLuint baseInstance;
};
