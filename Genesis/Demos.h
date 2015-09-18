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

GLuint loadCubemap(vector<const GLchar*>);
GLuint loadTexture(GLchar*);
void key_callback(GLFWwindow*, int, int, int, int);
void mouse_callback(GLFWwindow*, double, double);
void Do_Movement();
void generate_texture(float*, int, int);

void render_superbible_fragmentlist(GLFWwindow*);
void render_superbible_demo(GLFWwindow*);
void render_skybox_demo(GLFWwindow*);
void render_exploding_demo(GLFWwindow*);
void render_instancing_demo(GLFWwindow*);
