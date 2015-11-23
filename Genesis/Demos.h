#pragma once
// Std. Includes
#include "sb7ktx.h"
#include "sb7object.h"
#include "InputManager.h"
#include "Variables.h"

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>

extern genesis::InputManager inputManager;

GLuint loadCubemap(vector<const GLchar*>);
GLuint loadTexture(GLchar*);
static inline float random_float();

void render_superbible_shapedpoints(GLFWwindow*);
void render_superbible_starfield(GLFWwindow*);
void render_superbible_hdrtonemap(GLFWwindow*);
void render_superbible_polygonsmooth(GLFWwindow*);
void render_superbible_linesmooth(GLFWwindow*);
void render_superbible_basicfbo(GLFWwindow*);
void render_superbible_depthclamp(GLFWwindow*);
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