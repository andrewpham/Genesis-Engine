/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "ParticleGenerator.h"

namespace genesis {

	ParticleGenerator::ParticleGenerator(Shader _shader, Texture2D _texture, GLuint _amount)
		: _shader(_shader), _texture(_texture), _amount(_amount)
	{
		this->init();
	}

	void ParticleGenerator::update(GLfloat _dt, GameObject &_object, GLuint _newParticles, glm::vec2 _offset)
	{
		// Add new particles 
		for (GLuint i = 0; i < _newParticles; ++i)
		{
			int unusedParticle = this->firstUnusedParticle();
			this->respawnParticle(this->_particles[unusedParticle], _object, _offset);
		}
		// Update all particles
		for (GLuint i = 0; i < this->_amount; ++i)
		{
			Particle &p = this->_particles[i];
			p._life -= _dt; // reduce life
			if (p._life > 0.0f)
			{	// particle is alive, thus update
				p._position -= p._velocity * _dt;
				p._color.a -= _dt * 2.5;
			}
		}
	}

	// Render all particles
	void ParticleGenerator::draw()
	{
		// Use additive blending to give it a 'glow' effect
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		this->_shader.Use();
		for (Particle particle : this->_particles)
		{
			if (particle._life > 0.0f)
			{
				this->_shader.setVector2f("offset", particle._position);
				this->_shader.setVector4f("color", particle._color);
				this->_texture.bind();
				glBindVertexArray(this->VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
			}
		}
		// Don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void ParticleGenerator::init()
	{
		// Set up mesh and attribute properties
		GLuint VBO;
		GLfloat particle_quad[] = {
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(this->VAO);
		// Fill mesh buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
		// Set mesh attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);

		// Create this->amount default particle instances
		for (GLuint i = 0; i < this->_amount; ++i)
			this->_particles.push_back(Particle());
	}

	// Stores the index of the last particle used (for quick access to next dead particle)
	GLuint lastUsedParticle = 0;
	GLuint ParticleGenerator::firstUnusedParticle()
	{
		// First search from last used particle, this will usually return almost instantly
		for (GLuint i = lastUsedParticle; i < this->_amount; ++i) {
			if (this->_particles[i]._life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// Otherwise, do a linear search
		for (GLuint i = 0; i < lastUsedParticle; ++i) {
			if (this->_particles[i]._life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
		lastUsedParticle = 0;
		return 0;
	}

	void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
	{
		GLfloat random = ((rand() % 100) - 50) / 10.0f;
		GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
		particle._position = object._position + random + offset;
		particle._color = glm::vec4(rColor, rColor, rColor, 1.0f);
		particle._life = 1.0f;
		particle._velocity = object._velocity * 0.1f;
	}

}