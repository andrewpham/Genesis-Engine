/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "Variables.h"

namespace genesis {

	// ParticleGenerator acts as a container for rendering a large number of 
	// particles by repeatedly spawning and updating particles and killing 
	// them after a given amount of time.
	class ParticleGenerator
	{
	public:
		// Constructor
		ParticleGenerator(Shader _shader, Texture2D _texture, GLuint _amount);
		// Update all particles
		void update(GLfloat _dt, GameObject &_object, GLuint _newParticles, glm::vec2 _offset = glm::vec2(0.0f, 0.0f));
		// Render all particles
		void draw();
	private:
		// State
		std::vector<Particle> _particles;
		GLuint _amount;
		// Render state
		Shader _shader;
		Texture2D _texture;
		GLuint VAO;
		// Initializes buffer and vertex attributes
		void init();
		// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
		GLuint firstUnusedParticle();
		// Respawns particle
		void respawnParticle(Particle &_particle, GameObject &_object, glm::vec2 _offset = glm::vec2(0.0f, 0.0f));
	};

}