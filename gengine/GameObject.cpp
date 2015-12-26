/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "GameObject.h"

namespace genesis {

	GameObject::GameObject()
		: _position(0, 0), _size(1, 1), _velocity(0.0f), _color(1.0f), _rotation(0.0f), _sprite(), _isSolid(false), _destroyed(false) { }

	GameObject::GameObject(glm::vec2 _pos, glm::vec2 _size, Texture2D _sprite, glm::vec3 _color, glm::vec2 _velocity)
		: _position(_pos), _size(_size), _velocity(_velocity), _color(_color), _rotation(0.0f), _sprite(_sprite), _isSolid(false), _destroyed(false) { }

	void GameObject::draw(SpriteRenderer &renderer)
	{
		renderer.drawSprite(this->_sprite, this->_position, this->_size, this->_rotation, this->_color);
	}

}