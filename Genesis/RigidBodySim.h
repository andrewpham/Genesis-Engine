#pragma once
#include <gengine/Shader.h>
#include <gengine/InputManager.h>
#include <gengine/ResourceManager.h>
#include <gengine/Variables.h>
#include <gengine/GameObject3D.h>
#include <limits>

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>

extern genesis::InputManager _physicsSimInputManager;
extern genesis::ResourceManager _physicsSimResourceManager;
// Defines the normals associated with each cube
extern std::vector<glm::vec3> _normals;

void run_physics_demo(GLFWwindow*);