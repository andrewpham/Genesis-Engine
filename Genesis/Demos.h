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

void key_callback(GLFWwindow*, int, int, int, int);
void mouse_callback(GLFWwindow*, double, double);
void Do_Movement();
GLuint loadTexture(GLchar*);
GLuint loadCubemap(vector<const GLchar*>);

void render_skybox_demo(GLFWwindow*);
void render_exploding_demo(GLFWwindow*);
void render_instancing_demo(GLFWwindow*);
