#pragma once
#include <gengine/Shader.h>
#include <gengine/sb7ktx.h>
#include <gengine/sb7object.h>
#include <gengine/InputManager.h>
#include <gengine/ResourceManager.h>
#include <gengine/Variables.h>
#include <gengine/GameObject3D.h>
#include <gengine/Algorithms.h>

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

extern genesis::InputManager _skinDemoInputManager;
extern genesis::ResourceManager _skinDemoResourceManager;

void run_skin_demo(GLFWwindow*);