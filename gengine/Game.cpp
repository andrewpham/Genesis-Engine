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
	GameObject *Player;
	BallObject *Ball;

	Game::Game(GLuint _width, GLuint _height)
		: _state(GAME_ACTIVE), _keys(), _width(_width), _height(_height)
	{

	}

	Game::~Game()
	{
		delete Renderer;
		delete Player;
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
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/background.jpg", GL_FALSE, "background");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/awesomeface.png", GL_TRUE, "face");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/block.png", GL_FALSE, "block");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/block_solid.png", GL_FALSE, "block_solid");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/paddle.png", GL_TRUE, "paddle");
		// Set render-specific controls
		Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
		// Load levels
		GameLevel one; one.load("../Genesis/Levels/Breakout/one.lvl", this->_width, this->_height * 0.5);
		GameLevel two; two.load("../Genesis/Levels/Breakout/two.lvl", this->_width, this->_height * 0.5);
		GameLevel three; three.load("../Genesis/Levels/Breakout/three.lvl", this->_width, this->_height * 0.5);
		GameLevel four; four.load("../Genesis/Levels/Breakout/four.lvl", this->_width, this->_height * 0.5);
		this->_levels.push_back(one);
		this->_levels.push_back(two);
		this->_levels.push_back(three);
		this->_levels.push_back(four);
		this->_level = 0;
		// Configure game objects
		glm::vec2 playerPos = glm::vec2(this->_width / 2 - PLAYER_SIZE.x / 2, this->_height - PLAYER_SIZE.y);
		Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));
		glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
		Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
			ResourceManager::getTexture("face"));
	}

	void Game::update(GLfloat _dt)
	{
		Ball->move(_dt, this->_width);
	}


	void Game::processInput(GLfloat _dt)
	{
		if (this->_state == GAME_ACTIVE)
		{
			GLfloat velocity = PLAYER_VELOCITY * _dt;
			// Move playerboard
			if (this->_keys[GLFW_KEY_A])
			{
				if (Player->_position.x >= 0)
					Player->_position.x -= velocity;
				if (Ball->_stuck)
					Ball->_position.x -= velocity;
			}
			if (this->_keys[GLFW_KEY_D])
			{
				if (Player->_position.x <= this->_width - Player->_size.x)
					Player->_position.x += velocity;
				if (Ball->_stuck)
					Ball->_position.x += velocity;
			}
			if (this->_keys[GLFW_KEY_SPACE])
				Ball->_stuck = false;
		}
	}

	void Game::render()
	{
		if (this->_state == GAME_ACTIVE)
		{
			// Draw background
			Renderer->drawSprite(ResourceManager::getTexture("background"), glm::vec2(0, 0), glm::vec2(this->_width, this->_height), 0.0f);
			// Draw level
			this->_levels[this->_level].draw(*Renderer);
			// Draw player
			Player->draw(*Renderer);
			Ball->draw(*Renderer);
		}
	}

}