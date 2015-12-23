/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include "glm/gtc/matrix_transform.hpp"

#include "Texture2D.h"
#include "Shader.h"

namespace genesis {

	class SpriteRenderer
	{
	public:
		// Constructor (inits shaders/shapes)
		SpriteRenderer(Shader &_shader);
		// Destructor
		~SpriteRenderer();
		// Renders a defined quad textured with given sprite
		void drawSprite(Texture2D &_texture, glm::vec2 _position,
			glm::vec2 _size = glm::vec2(10, 10), GLfloat _rotate = 0.0f,
			glm::vec3 _color = glm::vec3(1.0f));
	private:
		// Render state
		Shader _shader;
		GLuint _quadVAO;
		// Initializes and configures the quad's buffer and vertex attributes
		void initRenderData();
	};

}