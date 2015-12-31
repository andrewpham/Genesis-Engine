/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#include "Texture2D.h"

namespace genesis {

	Texture2D::Texture2D()
		: _width(0), _height(0), _internalFormat(GL_RGB), _imageFormat(GL_RGB), _wrapS(GL_REPEAT), _wrapT(GL_REPEAT), _filterMin(GL_LINEAR), _filterMax(GL_LINEAR)
	{
		glGenTextures(1, &this->ID);
	}

	void Texture2D::generate(GLuint _width, GLuint _height, unsigned char* _data)
	{
		this->_width = _width;
		this->_height = _height;
		// Create Texture
		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, this->_internalFormat, _width, _height, 0, this->_imageFormat, GL_UNSIGNED_BYTE, _data);
		// Set Texture wrap and filter modes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->_wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->_wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->_filterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->_filterMax);
		// Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::bind() const
	{
		glBindTexture(GL_TEXTURE_2D, this->ID);
	}

}