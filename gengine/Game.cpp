/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "Game.h"

namespace genesis {

	// Game-related State data
	SpriteRenderer *Renderer;

	Game::Game(GLuint _width, GLuint _height)
		: _state(GAME_ACTIVE), _keys(), _width(_width), _height(_height)
	{

	}

	Game::~Game()
	{
		delete Renderer;
	}

	void Game::init()
	{
		// Load shaders
		ResourceManager::loadShader("../Genesis/Shaders/Breakout/sprite.vs", "../Genesis/Shaders/Breakout/sprite.frag", "sprite");
		// Configure shaders
		glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(_width), static_cast<GLfloat>(_height), 0.0f, -1.0f, 1.0f);
		ResourceManager::getShader("sprite").Use().setInteger("image", 0);
		ResourceManager::getShader("sprite").setMatrix4("projection", projection);
		// Load textures
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/awesomeface.png", GL_TRUE, "face");
		// Set render-specific controls
		Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
	}

	void Game::update(GLfloat dt)
	{

	}


	void Game::processInput(GLfloat dt)
	{

	}

	void Game::render()
	{
		Renderer->drawSprite(ResourceManager::getTexture("face"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	}

}