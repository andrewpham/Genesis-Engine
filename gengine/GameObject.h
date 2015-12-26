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
#include "SpriteRenderer.h"

namespace genesis {

	// Container object for holding all state relevant for a single
	// game object entity. Each object in the game likely needs the
	// minimal of state as described within GameObject.
	class GameObject
	{
	public:
		// Object state
		glm::vec2	_position, _size, _velocity;
		glm::vec3   _color;
		GLfloat     _rotation;
		GLboolean   _isSolid;
		GLboolean   _destroyed;
		// Render state
		Texture2D   _sprite;
		// Constructor(s)
		GameObject();
		GameObject(glm::vec2 _pos, glm::vec2 _size, Texture2D _sprite, glm::vec3 _color = glm::vec3(1.0f), glm::vec2 _velocity = glm::vec2(0.0f, 0.0f));
		// Draw sprite
		virtual void draw(SpriteRenderer &_renderer);
	};

}