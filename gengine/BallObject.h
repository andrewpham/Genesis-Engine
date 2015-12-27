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
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "PowerUp.h"

namespace genesis {

	// BallObject holds the state of the Ball object inheriting
	// relevant state data from GameObject. Contains some extra
	// functionality specific to Breakout's ball object that
	// were too specific for within GameObject alone.
	class BallObject :
		public GameObject
	{
	public:
		// Ball state
		GLfloat _radius;
		GLboolean _stuck;
		GLboolean _sticky, _passThrough;
		// Constructor(s)
		BallObject();
		BallObject(glm::vec2 _pos, GLfloat _radius, glm::vec2 _velocity, Texture2D _sprite);
		// Moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
		glm::vec2 move(GLfloat _dt, GLuint _windowWidth);
		// Resets the ball to original state with given position and velocity
		void      reset(glm::vec2 _position, glm::vec2 _velocity);
	};

}