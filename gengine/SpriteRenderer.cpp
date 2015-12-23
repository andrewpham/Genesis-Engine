/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "SpriteRenderer.h"

namespace genesis {

	SpriteRenderer::SpriteRenderer(Shader &_shader)
	{
		this->_shader = _shader;
		this->initRenderData();
	}


	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &this->_quadVAO);
	}

	void SpriteRenderer::drawSprite(Texture2D &_texture, glm::vec2 _position,
		glm::vec2 _size, GLfloat _rotate, glm::vec3 _color)
	{
		// Prepare transformations
		this->_shader.Use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(_position, 0.0f)); // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

		model = glm::translate(model, glm::vec3(0.5f * _size.x, 0.5f * _size.y, 0.0f)); // Move origin of rotation to center of quad
		model = glm::rotate(model, _rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
		model = glm::translate(model, glm::vec3(-0.5f * _size.x, -0.5f * _size.y, 0.0f)); // Move origin back

		model = glm::scale(model, glm::vec3(_size, 1.0f)); // Last scale

		this->_shader.setMatrix4("model", model);

		// Render textured quad
		this->_shader.setVector3f("spriteColor", _color);

		glActiveTexture(GL_TEXTURE0);
		_texture.bind();

		glBindVertexArray(this->_quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void SpriteRenderer::initRenderData()
	{
		// Configure VAO/VBO
		GLuint VBO;
		GLfloat vertices[] = {
			// Pos      // Tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &this->_quadVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(this->_quadVAO);;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}