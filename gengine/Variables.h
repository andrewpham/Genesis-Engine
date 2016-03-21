#pragma once

#define PI_F			3.14159265358979f
//#define WINDOW_NAME		"Rigid Body Sim"
#define WINDOW_NAME		"Realistic Skin"
#define SCREEN_WIDTH	1920
#define SCREEN_HEIGHT	1080
/** Variables pertaining to the OpenGL SuperBible Demos */
#define NUM_DRAWS		50000
#define WORKGROUP_SIZE	64
#define NUM_WORKGROUPS	16
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
// The size of a PowerUp block
#define SIZE			glm::vec2(60, 20)
// Velocity a PowerUp block has when spawned
#define VELOCITY		glm::vec2(0.0f, 150.0f)
/** Variables pertaining to Life of Gaben */
// Enemy DPS
#define DAMAGE			10
// Light position
static const glm::vec3	LIGHT_POS(0.0f, 1.0f, 4.0f);
// How frequently in seconds you can spawn towers
#define TOWER_SPAWN_RATE	15
/** Variables pertaining to Rigid Body Sim */
// Gravitational acceleration
static const glm::vec3	G_CONST(0.0f, -0.15f, 0.0f);

/** Enumerations and definitions pertaining to the OpenGL SuperBible Demos */

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

/** Enumerations and definitions pertaining to Life of Gaben */

// Represents a single fish in a 3D flock and its state
struct Fish
{
	glm::vec3 position;
	unsigned int : 32;
	glm::vec3 velocity;
	unsigned int : 32;
};

/** Vertex data pertaining to canonical shapes in Life of Gaben */

static const GLfloat BOX_VERTICES[] = {
	// Positions          // Normals           // Texture Coords
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

static const GLfloat SKYBOX_VERTICES[] = {
	// Positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

static const glm::vec3 FISH_VERTICES[] =
{
	// Positions
	glm::vec3(-5.0f, 1.0f, 0.0f) * 0.2f,
	glm::vec3(-1.0f, 1.5f, 0.0f) * 0.2f,
	glm::vec3(-1.0f, 1.5f, 7.0f) * 0.2f,
	glm::vec3(0.0f, 0.0f, 0.0f) * 0.2f,
	glm::vec3(0.0f, 0.0f, 10.0f) * 0.2f,
	glm::vec3(1.0f, 1.5f, 0.0f) * 0.2f,
	glm::vec3(1.0f, 1.5f, 7.0f) * 0.2f,
	glm::vec3(5.0f, 1.0f, 0.0f) * 0.2f,

	// Normals
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.107f, -0.859f, 0.00f),
	glm::vec3(0.832f, 0.554f, 0.00f),
	glm::vec3(-0.59f, -0.395f, 0.00f),
	glm::vec3(-0.832f, 0.554f, 0.00f),
	glm::vec3(0.295f, -0.196f, 0.00f),
	glm::vec3(0.124f, 0.992f, 0.00f),
};