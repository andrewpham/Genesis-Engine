/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "SpriteRenderer.h"
#include "Shader.h"

namespace genesis {

	// PostProcessor hosts all PostProcessing effects for the Breakout
	// Game. It renders the game on a textured quad after which one can
	// enable specific effects by enabling either the Confuse, Chaos or 
	// Shake boolean. 
	// It is required to call BeginRender() before rendering the game
	// and EndRender() after rendering the game for the class to work.
	class PostProcessor
	{
	public:
		// State
		Shader _postProcessingShader;
		Texture2D _texture;
		GLuint _width, _height;
		// Options
		GLboolean _confuse, _chaos, _shake;
		// Constructor
		PostProcessor(Shader _shader, GLuint _width, GLuint _height);
		// Prepares the postprocessor's framebuffer operations before rendering the game
		void beginRender();
		// Should be called after rendering the game, so it stores all the rendered data into a texture object
		void endRender();
		// Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
		void render(GLfloat _time);
	private:
		// Render state
		GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
		GLuint RBO; // RBO is used for multisampled color buffer
		GLuint VAO;
		// Initialize quad for rendering postprocessing texture
		void initRenderData();
	};

}