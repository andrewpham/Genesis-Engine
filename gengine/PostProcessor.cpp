/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "PostProcessor.h"

namespace genesis {

	PostProcessor::PostProcessor(Shader _shader, GLuint _width, GLuint _height)
		: _postProcessingShader(_shader), _texture(), _width(_width), _height(_height), _confuse(GL_FALSE), _chaos(GL_FALSE), _shake(GL_FALSE)
	{
		// Initialize renderbuffer/framebuffer object
		glGenFramebuffers(1, &this->MSFBO);
		glGenFramebuffers(1, &this->FBO);
		glGenRenderbuffers(1, &this->RBO);

		// Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
		glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, _width, _height); // Allocate storage for render buffer object
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // Attach MS render buffer object to framebuffer
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

		// Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
		this->_texture.generate(_width, _height, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_texture.ID, 0); // Attach texture to framebuffer as its color attachment
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Initialize render data and uniforms
		this->initRenderData();
		this->_postProcessingShader.setInteger("scene", 0, GL_TRUE);
		GLfloat offset = 1.0f / 300.0f;
		GLfloat offsets[9][2] = {
			{ -offset,  offset },  // top-left
			{ 0.0f,    offset },  // top-center
			{ offset,  offset },  // top-right
			{ -offset,  0.0f },  // center-left
			{ 0.0f,    0.0f },  // center-center
			{ offset,  0.0f },  // center - right
			{ -offset, -offset },  // bottom-left
			{ 0.0f,   -offset },  // bottom-center
			{ offset, -offset }   // bottom-right    
		};
		glUniform2fv(glGetUniformLocation(this->_postProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);
		GLint edge_kernel[9] = {
			-1, -1, -1,
			-1,  8, -1,
			-1, -1, -1
		};
		glUniform1iv(glGetUniformLocation(this->_postProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
		GLfloat blur_kernel[9] = {
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16
		};
		glUniform1fv(glGetUniformLocation(this->_postProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
	}

	void PostProcessor::beginRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void PostProcessor::endRender()
	{
		// Now resolve multisampled color-buffer into intermediate FBO to store to texture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
		glBlitFramebuffer(0, 0, this->_width, this->_height, 0, 0, this->_width, this->_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer
	}

	void PostProcessor::render(GLfloat time)
	{
		// Set uniforms/options
		this->_postProcessingShader.Use();
		this->_postProcessingShader.setFloat("time", time);
		this->_postProcessingShader.setInteger("confuse", this->_confuse);
		this->_postProcessingShader.setInteger("chaos", this->_chaos);
		this->_postProcessingShader.setInteger("shake", this->_shake);
		// Render textured quad
		glActiveTexture(GL_TEXTURE0);
		this->_texture.bind();
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void PostProcessor::initRenderData()
	{
		// Configure VAO/VBO
		GLuint VBO;
		GLfloat vertices[] = {
			// Pos        // Tex
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,

			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f,  1.0f, 1.0f, 1.0f
		};
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(this->VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}