/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "InputManager.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "BallObject.h"
#include "ParticleGenerator.h"

namespace genesis {

	// Game holds all game-related state and functionality.
	// Combines all game-related data into a single class for
	// easy access to each of the components and manageability.
	class Game
	{
	public:
		// Game state
		GameState              _state;
		GLboolean              _keys[1024];
		GLuint                 _width, _height;
		std::vector<GameLevel> _levels;
		GLuint				   _level;
		// Constructor/Destructor
		Game(GLuint, GLuint);
		~Game();
		// Initialize game state (load all shaders/textures/levels)
		void init();
		// GameLoop
		void processInput(GLfloat);
		void update(GLfloat);
		void render();
		void doCollisions();
		// Reset
		void resetLevel();
		void resetPlayer();
		// Collision detection
		GLboolean checkCollision(GameObject &_one, GameObject &_two);
		Collision checkCollision(BallObject &_one, GameObject &_two);
		Direction vectorDirection(glm::vec2 _closest);
	};

}