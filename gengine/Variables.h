#pragma once

#define PI_F			3.14159265358979f
#define WINDOW_NAME		"Genesis"
#define GAME_NAME		"Breakout"
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define NUM_DRAWS		50000
#define WORKGROUP_SIZE	256
#define NUM_WORKGROUPS	64
#define FLOCK_SIZE		NUM_WORKGROUPS * WORKGROUP_SIZE
//#define MANY_OBJECTS	1
// Initial size of the player paddle
#define PLAYER_SIZE		glm::vec2(100, 20)
// Initial velocity of the player paddle
#define PLAYER_VELOCITY	500.0f
// Initial velocity of the Ball
#define INITIAL_BALL_VELOCITY glm::vec2(100.0f, -350.0f)
// Radius of the ball object
#define BALL_RADIUS		12.5f

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

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};
