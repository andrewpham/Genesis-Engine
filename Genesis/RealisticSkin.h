#pragma once
#include <gengine/Shader.h>
#include <gengine/InputManager.h>
#include <gengine/ResourceManager.h>

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>

extern genesis::InputManager _skinDemoInputManager;
extern genesis::ResourceManager _skinDemoResourceManager;

void run_skin_demo(GLFWwindow*);