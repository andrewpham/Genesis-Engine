/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Variables.h"

namespace genesis {

	// PowerUp inherits its state and rendering functions from
	// GameObject but also holds extra information to state its
	// active duration and whether it is activated or not. 
	// The type of PowerUp is stored as a string.
	class PowerUp : public GameObject
	{
	public:
		// PowerUp State
		std::string _type;
		GLfloat     _duration;
		GLboolean   _activated;
		// Constructor
		PowerUp(std::string _type, glm::vec3 _color, GLfloat _duration, glm::vec2 _position, Texture2D _texture)
			: GameObject(_position, SIZE, _texture, _color, VELOCITY), _type(_type), _duration(_duration), _activated() { }
	};

}