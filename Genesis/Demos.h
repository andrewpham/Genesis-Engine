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

// Multi-Draw Indirect Enums and Struct
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

void render_superbible_multiscissor(GLFWwindow*);
void render_superbible_noperspective(GLFWwindow*);
void render_superbible_multiviewport(GLFWwindow*);
void render_superbible_gsquads(GLFWwindow*);
void render_superbible_normalviewer(GLFWwindow*);
void render_superbible_gstessellate(GLFWwindow*);
void render_superbible_objectexploder(GLFWwindow*);
void render_superbible_gsculling(GLFWwindow*);
void render_superbible_cubicbezier(GLFWwindow*);
void render_superbible_dispmap(GLFWwindow*);
void render_superbible_tessmodes(GLFWwindow*);
void render_superbible_clipdistance(GLFWwindow*);
void render_superbible_multidrawindirect(GLFWwindow*);
void render_superbible_instancedattribs(GLFWwindow*);
void render_superbible_fragmentlist(GLFWwindow*);
void render_skybox_demo(GLFWwindow*);
void render_exploding_demo(GLFWwindow*);
void render_instancing_demo(GLFWwindow*);
