/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ResourceManager.h"

namespace genesis {

	// Represents the current state of the game
	enum GameState 
	{
		GAME_ACTIVE,
		GAME_MENU,
		GAME_WIN
	};

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
		// Constructor/Destructor
		Game(GLuint, GLuint);
		~Game();
		// Initialize game state (load all shaders/textures/levels)
		void init();
		// GameLoop
		void processInput(GLfloat);
		void update(GLfloat);
		void render();
	};

}

#endif