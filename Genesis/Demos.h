#pragma once
// Std. Includes
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

// GLFW
#include <GLFW/glfw3.h>

// Properties
extern GLuint screenWidth, screenHeight;

enum
{
	NUM_DRAWS = 50000
};
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

GLuint loadCubemap(vector<const GLchar*>);
GLuint loadTexture(GLchar*);
void key_callback(GLFWwindow*, int, int, int, int);
void mouse_callback(GLFWwindow*, double, double);
void Do_Movement();
void generate_texture(float*, int, int);

void render_superbible_asteroids(GLFWwindow*);
void render_superbible_squares(GLFWwindow*);
void render_superbible_fragmentlist(GLFWwindow*);
void render_superbible_demo(GLFWwindow*);
void render_skybox_demo(GLFWwindow*);
void render_exploding_demo(GLFWwindow*);
void render_instancing_demo(GLFWwindow*);
