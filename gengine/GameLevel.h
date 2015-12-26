/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once
#include <vector>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"


namespace genesis {

	/// GameLevel holds all Tiles as part of a Breakout level and 
	/// hosts functionality to Load/render levels from the harddisk.
	class GameLevel
	{
	public:
		// Level state
		std::vector<GameObject> _bricks;
		// Constructor
		GameLevel() { }
		// Loads level from file
		void      load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
		// Render level
		void      draw(SpriteRenderer &renderer);
		// Check if the level is completed (all non-solid tiles are destroyed)
		GLboolean isCompleted();
	private:
		// Initialize level from tile data
		void      init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
	};

}