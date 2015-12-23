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
	enum GameState {
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
		GameState              State;
		GLboolean              Keys[1024];
		GLuint                 Width, Height;
		// Constructor/Destructor
		Game(GLuint width, GLuint height);
		~Game();
		// Initialize game state (load all shaders/textures/levels)
		void Init();
		// GameLoop
		void ProcessInput(GLfloat dt);
		void Update(GLfloat dt);
		void Render();
	};

}

#endif