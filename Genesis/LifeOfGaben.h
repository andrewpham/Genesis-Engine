#pragma once
#include <vector>

#include <gengine/Shader.h>
#include <gengine/sb7ktx.h>
#include <gengine/sb7object.h>
#include <gengine/InputManager.h>
#include <gengine/ResourceManager.h>
#include <gengine/Variables.h>
#include <gengine/GameObject3D.h>

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>

extern genesis::InputManager _gabenGameInputManager;
extern genesis::ResourceManager _gabenGameResourceManager;

void run_gaben_game(GLFWwindow*);
