#pragma once
#include <gengine/Shader.h>
#include <gengine/sb7ktx.h>
#include <gengine/sb7object.h>
#include <gengine/InputManager.h>
#include <gengine/ResourceManager.h>
#include <gengine/Variables.h>
#include <gengine/GameObject3D.h>
#include <gengine/Enemy.h>

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

extern genesis::InputManager _gabenGameInputManager;
extern genesis::ResourceManager _gabenGameResourceManager;
// Health of the player
extern GLfloat _health;
// Used to pace the triggering of the audio engine sounds at regular intervals
extern GLfloat _secondsSinceDamaged;

void run_gaben_game(GLFWwindow*);
