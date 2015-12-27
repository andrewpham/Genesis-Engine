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
	SpriteRenderer    *Renderer;
	GameObject        *Player;
	BallObject        *Ball;
	ParticleGenerator *Particles;
	PostProcessor	  *Effects;
	ISoundEngine      *SoundEngine = createIrrKlangDevice();
	GLfloat			   ShakeTime = 0.0f;

	Game::Game(GLuint _width, GLuint _height)
		: _state(GAME_ACTIVE), _keys(), _width(_width), _height(_height)
	{

	}

	Game::~Game()
	{
		delete Renderer;
		delete Player;
		delete Ball;
		delete Particles;
		delete Effects;
		SoundEngine->drop();
	}

	void Game::init()
	{
		// Load shaders
		ResourceManager::loadShader("../Genesis/Shaders/Breakout/sprite.vs", "../Genesis/Shaders/Breakout/sprite.frag", "sprite");
		ResourceManager::loadShader("../Genesis/Shaders/Breakout/particle.vs", "../Genesis/Shaders/Breakout/particle.frag", "particle");
		ResourceManager::loadShader("../Genesis/Shaders/Breakout/post_processing.vs", "../Genesis/Shaders/Breakout/post_processing.frag", "postprocessing");
		// Configure shaders
		glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->_width), static_cast<GLfloat>(this->_height), 0.0f, -1.0f, 1.0f);
		ResourceManager::getShader("sprite").Use().setInteger("sprite", 0);
		ResourceManager::getShader("sprite").setMatrix4("projection", projection);
		ResourceManager::getShader("particle").Use().setInteger("sprite", 0);
		ResourceManager::getShader("particle").setMatrix4("projection", projection);
		// Load textures
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/background.jpg", GL_FALSE, "background");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/ball.png", GL_TRUE, "ball");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/block.png", GL_FALSE, "block");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/block_solid.png", GL_FALSE, "block_solid");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/paddle.png", GL_TRUE, "paddle");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/particle.png", GL_TRUE, "particle");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/powerup_speed.png", GL_TRUE, "powerup_speed");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/powerup_sticky.png", GL_TRUE, "powerup_sticky");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/powerup_increase.png", GL_TRUE, "powerup_increase");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/powerup_confuse.png", GL_TRUE, "powerup_confuse");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/powerup_chaos.png", GL_TRUE, "powerup_chaos");
		ResourceManager::loadTexture("../Genesis/Textures/Breakout/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
		// Set render-specific controls
		Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
		Particles = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getTexture("particle"), 500);
		Effects = new PostProcessor(ResourceManager::getShader("postprocessing"), this->_width, this->_height);
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
		Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("ball"));
		// Audio
		SoundEngine->play2D("../Genesis/Audio/Breakout/meteor.mp3", GL_TRUE);
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
				{
					Player->_position.x -= velocity;
					if (Ball->_stuck)
						Ball->_position.x -= velocity;
				}
			}
			if (this->_keys[GLFW_KEY_D])
			{
				if (Player->_position.x <= this->_width - Player->_size.x)
				{
					Player->_position.x += velocity;
					if (Ball->_stuck)
						Ball->_position.x += velocity;
				}
			}
			if (this->_keys[GLFW_KEY_SPACE])
				Ball->_stuck = false;
		}
	}

	void Game::update(GLfloat _dt)
	{
		// Update objects
		Ball->move(_dt, this->_width);
		// Check for collisions
		this->doCollisions();
		// Update particles
		Particles->update(_dt, *Ball, 2, glm::vec2(Ball->_radius / 2));
		// Reduce shake time
		// Update PowerUps
		this->updatePowerUps(_dt);
		if (ShakeTime > 0.0f)
		{
			ShakeTime -= _dt;
			if (ShakeTime <= 0.0f)
				Effects->_shake = GL_FALSE;
		}
		// Check loss condition
		if (Ball->_position.y >= this->_height) // Did ball reach bottom edge?
		{
			this->resetLevel();
			this->resetPlayer();
		}
	}

	void Game::render()
	{
		if (this->_state == GAME_ACTIVE)
		{
			// Begin rendering to postprocessing quad
			Effects->beginRender();
			// Draw background
			Renderer->drawSprite(ResourceManager::getTexture("background"), glm::vec2(0, 0), glm::vec2(this->_width, this->_height), 0.0f);
			// Draw level
			this->_levels[this->_level].draw(*Renderer);
			// Draw player
			Player->draw(*Renderer);
			// Draw PowerUps
			for (PowerUp &powerUp : this->_powerUps)
				if (!powerUp._destroyed)
					powerUp.draw(*Renderer);
			// Draw particles	
			Particles->draw();
			// Draw ball
			Ball->draw(*Renderer);
			// End rendering to postprocessing quad
			Effects->endRender();
			// Render postprocessing quad
			Effects->render(glfwGetTime());
		}
	}

	// Collision detection
	void Game::doCollisions()
	{
		for (GameObject &box : this->_levels[this->_level]._bricks)
		{
			if (!box._destroyed)
			{
				Collision collision = checkCollision(*Ball, box);
				if (std::get<0>(collision)) // If collision is true
				{
					// Destroy block if not solid
					if (!box._isSolid)
					{
						box._destroyed = GL_TRUE;
						this->spawnPowerUps(box);
						SoundEngine->play2D("../Genesis/Audio/Breakout/bleep.mp3", GL_FALSE);
					}
					else
					{   // if block is solid, enable shake effect
						ShakeTime = 0.05f;
						Effects->_shake = GL_TRUE;
						SoundEngine->play2D("../Genesis/Audio/Breakout/solid.wav", GL_FALSE);
					}
					// Collision resolution
					Direction dir = std::get<1>(collision);
					glm::vec2 diff_vector = std::get<2>(collision);
					if (!(Ball->_passThrough && !box._isSolid)) // don't do collision resolution on non-solid bricks if pass-through activated
					{
						if (dir == LEFT || dir == RIGHT) // Horizontal collision
						{
							Ball->_velocity.x = -Ball->_velocity.x; // Reverse horizontal velocity
																  // Relocate
							GLfloat penetration = Ball->_radius - std::abs(diff_vector.x);
							if (dir == LEFT)
								Ball->_position.x += penetration; // Move ball to right
							else
								Ball->_position.x -= penetration; // Move ball to left;
						}
						else // Vertical collision
						{
							Ball->_velocity.y = -Ball->_velocity.y; // Reverse vertical velocity
																  // Relocate
							GLfloat penetration = Ball->_radius - std::abs(diff_vector.y);
							if (dir == UP)
								Ball->_position.y -= penetration; // Move ball bback up
							else
								Ball->_position.y += penetration; // Move ball back down
						}
					}
				}
			}
		}

		// Also check collisions on PowerUps and if so, activate them
		for (PowerUp &powerUp : this->_powerUps)
		{
			if (!powerUp._destroyed)
			{
				// First check if powerup passed bottom edge, if so: keep as inactive and destroy
				if (powerUp._position.y >= this->_height)
					powerUp._destroyed = GL_TRUE;

				if (checkCollision(*Player, powerUp))
				{	// Collided with player, now activate powerup
					activatePowerUp(powerUp);
					powerUp._destroyed = GL_TRUE;
					powerUp._activated = GL_TRUE;
					SoundEngine->play2D("../Genesis/Audio/Breakout/powerup.wav", GL_FALSE);
				}
			}
		}

		// Also check collisions for player pad (unless stuck)
		Collision result = checkCollision(*Ball, *Player);
		if (!Ball->_stuck && std::get<0>(result))
		{
			// Check where it hit the board, and change velocity based on where it hit the board
			GLfloat centerBoard = Player->_position.x + Player->_size.x / 2;
			GLfloat distance = (Ball->_position.x + Ball->_radius) - centerBoard;
			GLfloat percentage = distance / (Player->_size.x / 2);
			// Then move accordingly
			GLfloat strength = 2.0f;
			glm::vec2 oldVelocity = Ball->_velocity;
			Ball->_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
			//Ball->Velocity.y = -Ball->Velocity.y;
			Ball->_velocity = glm::normalize(Ball->_velocity) * glm::length(oldVelocity); // Keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
																						// Fix sticky paddle
			Ball->_velocity.y = -1 * abs(Ball->_velocity.y);

			// If Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
			Ball->_stuck = Ball->_sticky;

			SoundEngine->play2D("../Genesis/Audio/Breakout/bleep.wav", GL_FALSE);
		}
	}

	void Game::resetLevel()
	{
		if (this->_level == 0)this->_levels[0].load("../Genesis/Levels/Breakout/one.lvl", this->_width, this->_height * 0.5f);
		else if (this->_level == 1)
			this->_levels[1].load("../Genesis/Levels/Breakout/two.lvl", this->_width, this->_height * 0.5f);
		else if (this->_level == 2)
			this->_levels[2].load("../Genesis/Levels/Breakout/three.lvl", this->_width, this->_height * 0.5f);
		else if (this->_level == 3)
			this->_levels[3].load("../Genesis/Levels/Breakout/four.lvl", this->_width, this->_height * 0.5f);
	}

	void Game::resetPlayer()
	{
		// Reset player/ball stats
		Player->_size = PLAYER_SIZE;
		Player->_position = glm::vec2(this->_width / 2 - PLAYER_SIZE.x / 2, this->_height - PLAYER_SIZE.y);
		Ball->reset(Player->_position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
		// Also disable all active powerups
		Effects->_chaos = Effects->_confuse = GL_FALSE;
		Ball->_passThrough = Ball->_sticky = GL_FALSE;
		Player->_color = glm::vec3(1.0f);
		Ball->_color = glm::vec3(1.0f);
	}

	// PowerUps
	void Game::spawnPowerUps(GameObject &_block)
	{
		if (shouldSpawn(75)) // 1 in 75 chance
			this->_powerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, _block._position, ResourceManager::getTexture("powerup_speed")));
		if (shouldSpawn(75))
			this->_powerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, _block._position, ResourceManager::getTexture("powerup_sticky")));
		if (shouldSpawn(75))
			this->_powerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, _block._position, ResourceManager::getTexture("powerup_passthrough")));
		if (shouldSpawn(75))
			this->_powerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, _block._position, ResourceManager::getTexture("powerup_increase")));
		if (shouldSpawn(15)) // Negative powerups should spawn more often
			this->_powerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, _block._position, ResourceManager::getTexture("powerup_confuse")));
		if (shouldSpawn(15))
			this->_powerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, _block._position, ResourceManager::getTexture("powerup_chaos")));
	}

	void Game::updatePowerUps(GLfloat _dt)
	{
		for (PowerUp &powerUp : this->_powerUps)
		{
			powerUp._position += powerUp._velocity * _dt;
			if (powerUp._activated)
			{
				powerUp._duration -= _dt;

				if (powerUp._duration <= 0.0f)
				{
					// Remove powerup from list (will later be removed)
					powerUp._activated = GL_FALSE;
					// Deactivate effects
					if (powerUp._type == "sticky")
					{
						if (!isOtherPowerUpActive(this->_powerUps, "sticky"))
						{	// Only reset if no other PowerUp of type sticky is active
							Ball->_sticky = GL_FALSE;
							Player->_color = glm::vec3(1.0f);
						}
					}
					else if (powerUp._type == "pass-through")
					{
						if (!isOtherPowerUpActive(this->_powerUps, "pass-through"))
						{	// Only reset if no other PowerUp of type pass-through is active
							Ball->_passThrough = GL_FALSE;
							Ball->_color = glm::vec3(1.0f);
						}
					}
					else if (powerUp._type == "confuse")
					{
						if (!isOtherPowerUpActive(this->_powerUps, "confuse"))
						{	// Only reset if no other PowerUp of type confuse is active
							Effects->_confuse = GL_FALSE;
						}
					}
					else if (powerUp._type == "chaos")
					{
						if (!isOtherPowerUpActive(this->_powerUps, "chaos"))
						{	// Only reset if no other PowerUp of type chaos is active
							Effects->_chaos = GL_FALSE;
						}
					}
				}
			}
		}
		// Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
		// Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
		this->_powerUps.erase(std::remove_if(this->_powerUps.begin(), this->_powerUps.end(),
			[](const PowerUp &powerUp) { return powerUp._destroyed && !powerUp._activated; }
		), this->_powerUps.end());
	}

	GLboolean Game::checkCollision(GameObject &_one, GameObject &_two) // AABB - AABB collision
	{
		// Collision x-axis?
		bool collisionX = _one._position.x + _one._size.x >= _two._position.x &&
			_two._position.x + _two._size.x >= _one._position.x;
		// Collision y-axis?
		bool collisionY = _one._position.y + _one._size.y >= _two._position.y &&
			_two._position.y + _two._size.y >= _one._position.y;
		// Collision only if on both axes
		return collisionX && collisionY;
	}

	Collision Game::checkCollision(BallObject &_one, GameObject &_two) // AABB - Circle collision
	{
		// Get center point circle first 
		glm::vec2 center(_one._position + _one._radius);
		// Calculate AABB info (center, half-extents)
		glm::vec2 aabb_half_extents(_two._size.x / 2, _two._size.y / 2);
		glm::vec2 aabb_center(_two._position.x + aabb_half_extents.x, _two._position.y + aabb_half_extents.y);
		// Get difference vector between both centers
		glm::vec2 difference = center - aabb_center;
		glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
		// Now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
		glm::vec2 closest = aabb_center + clamped;
		// Now retrieve vector between center circle and closest point AABB and check if length < radius
		difference = closest - center;

		if (glm::length(difference) < _one._radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
			return std::make_tuple(GL_TRUE, vectorDirection(difference), difference);
		else
			return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
	}

	// Calculates which direction a vector is facing (N,E,S or W)
	Direction Game::vectorDirection(glm::vec2 _target)
	{
		glm::vec2 compass[] = {
			glm::vec2(0.0f, 1.0f),	// up
			glm::vec2(1.0f, 0.0f),	// right
			glm::vec2(0.0f, -1.0f),	// down
			glm::vec2(-1.0f, 0.0f)	// left
		};
		GLfloat max = 0.0f;
		GLuint best_match = -1;
		for (GLuint i = 0; i < 4; i++)
		{
			GLfloat dot_product = glm::dot(glm::normalize(_target), compass[i]);
			if (dot_product > max)
			{
				max = dot_product;
				best_match = i;
			}
		}
		return (Direction)best_match;
	}

	GLboolean Game::isOtherPowerUpActive(std::vector<PowerUp> &_powerUps, std::string _type)
	{
		// Check if another PowerUp of the same type is still active
		// in which case we don't disable its effect (yet)
		for (const PowerUp &powerUp : _powerUps)
		{
			if (powerUp._activated)
				if (powerUp._type == _type)
					return GL_TRUE;
		}
		return GL_FALSE;
	}

	GLboolean Game::shouldSpawn(GLuint _chance)
	{
		GLuint random = rand() % _chance;
		return random == 0;
	}

	void Game::activatePowerUp(PowerUp &_powerUp)
	{
		// Initiate a powerup based type of powerup
		if (_powerUp._type == "speed")
		{
			Ball->_velocity *= 1.2;
		}
		else if (_powerUp._type == "sticky")
		{
			Ball->_sticky = GL_TRUE;
			Player->_color = glm::vec3(1.0f, 0.5f, 1.0f);
		}
		else if (_powerUp._type == "pass-through")
		{
			Ball->_passThrough = GL_TRUE;
			Ball->_color = glm::vec3(1.0f, 0.5f, 0.5f);
		}
		else if (_powerUp._type == "pad-size-increase")
		{
			Player->_size.x += 50;
		}
		else if (_powerUp._type == "confuse")
		{
			if (!Effects->_chaos)
				Effects->_confuse = GL_TRUE; // Only activate if chaos wasn't already active
		}
		else if (_powerUp._type == "chaos")
		{
			if (!Effects->_confuse)
				Effects->_chaos = GL_TRUE;
		}
	}

}