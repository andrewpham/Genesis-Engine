#pragma once
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gengine/Game.h>
#include <gengine/ResourceManager.h>
#include <gengine/Variables.h>

extern genesis::ResourceManager _breakoutResourceManager;
extern genesis::Game Breakout;

void play_breakout(GLFWwindow*);
