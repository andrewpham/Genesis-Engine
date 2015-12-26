/******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "BallObject.h"


namespace genesis {

	BallObject::BallObject()
		: GameObject(), _radius(12.5f), _stuck(true) { }

	BallObject::BallObject(glm::vec2 _pos, GLfloat _radius, glm::vec2 _velocity, Texture2D _sprite)
		: GameObject(_pos, glm::vec2(_radius * 2, _radius * 2), _sprite, glm::vec3(1.0f), _velocity), _radius(_radius), _stuck(true) { }

	glm::vec2 BallObject::move(GLfloat _dt, GLuint _windowWidth)
	{
		// If not stuck to player board
		if (!this->_stuck)
		{
			// Move the ball
			this->_position += this->_velocity * _dt;
			// Then check if outside window bounds and if so, reverse velocity and restore at correct position
			if (this->_position.x <= 0.0f)
			{
				this->_velocity.x = -this->_velocity.x;
				this->_position.x = 0.0f;
			}
			else if (this->_position.x + this->_size.x >= _windowWidth)
			{
				this->_velocity.x = -this->_velocity.x;
				this->_position.x = _windowWidth - this->_size.x;
			}
			if (this->_position.y <= 0.0f)
			{
				this->_velocity.y = -this->_velocity.y;
				this->_position.y = 0.0f;
			}
		}
		return this->_position;
	}

	// Resets the ball to initial Stuck Position (if ball is outside window bounds)
	void BallObject::reset(glm::vec2 _position, glm::vec2 _velocity)
	{
		this->_position = _position;
		this->_velocity = _velocity;
		this->_stuck = true;
	}

}