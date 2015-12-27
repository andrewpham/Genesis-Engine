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
/** Variables pertaining to the game Breakout */
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

/** Enumerations and definitions pertaining to the game Breakout */

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Represents the four possible (collision) directions
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Defines a Collision typedef that represents collision data
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>

// Represents a single particle and its state
struct Particle {
	glm::vec2 _position, _velocity;
	glm::vec4 _color;
	GLfloat _life;

	Particle() : _position(0.0f), _velocity(0.0f), _color(1.0f), _life(0.0f) { }
};