// GL includes
#include <gengine/Model.h>
#include "LifeOfGaben.h"

genesis::InputManager _gabenGameInputManager;
genesis::ResourceManager _gabenGameResourceManager;
static GLfloat _health = 100.0f;
static GLfloat _secondsSinceDamaged = 0.0f;

Direction vectorDirection(glm::vec2);
glm::quat rotationBetweenVectors(glm::vec3, glm::vec3);
GLboolean checkCollision(genesis::GameObject3D&, genesis::InputManager&);
void resolveCollision(genesis::GameObject3D&, genesis::InputManager&);
void resolveEnemyInteractions(genesis::Enemy&, genesis::InputManager&, GLfloat, GLuint);
void resolveWallCollisions(GLfloat, GLfloat, GLfloat, GLfloat, genesis::InputManager&);

static inline float random_float()
{
	static unsigned int seed = 0x13371337;

	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

// Courtesy of the fine people over at StackOverflow
static inline float random_range(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void run_gaben_game(GLFWwindow* window)
{
	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup and compile our shaders
	genesis::Shader shader = _gabenGameResourceManager.loadShader("Shaders/Life of Gaben/object.vs", "Shaders/Life of Gaben/object.frag", "object");
	genesis::Shader skyboxShader = _gabenGameResourceManager.loadShader("Shaders/Life of Gaben/skybox.vs", "Shaders/Life of Gaben/skybox.frag", "skybox");
	genesis::Shader flockShader = _gabenGameResourceManager.loadShader("Shaders/Life of Gaben/flock.vs", "Shaders/Life of Gaben/flock.frag", "flock");
	genesis::Shader flockUpdateShader = _gabenGameResourceManager.loadShader("Shaders/Life of Gaben/flock.comp", "flockUpdate");

	// Flock data and state
	GLuint flockBuffers[2], flockVAO[2], geometryVBO;
	// Box data and state
	GLuint boxVAO, boxVBO;
	// Floor data and state
	GLuint floorVAO, floorVBO;
	// Wall data and state
	GLuint wallVAO, wallVBO;
	// Skybox data and state
	GLuint skyboxVAO, skyboxVBO;

	// Uniforms for the various shaders
	struct
	{
		struct
		{
			GLint view;
			GLint projection;
		} object;
		struct
		{
			GLint view;
			GLint projection;
		} skybox;
		struct
		{
			GLuint mvp;
		} flock;
		struct
		{
			GLint goal;
		} flockUpdate;
	} uniforms;

	// Cache the uniform locations
	shader.Use();
	uniforms.object.view = glGetUniformLocation(shader.ID, "view");
	uniforms.object.projection = glGetUniformLocation(shader.ID, "projection");
	skyboxShader.Use();
	uniforms.skybox.view = glGetUniformLocation(skyboxShader.ID, "view");
	uniforms.skybox.projection = glGetUniformLocation(skyboxShader.ID, "projection");
	flockShader.Use();
	uniforms.flock.mvp = glGetUniformLocation(flockShader.ID, "mvp");
	flockUpdateShader.Use();
	uniforms.flockUpdate.goal = glGetUniformLocation(flockUpdateShader.ID, "goal");

	// Index to swap flock buffers with one another to be used in the rendering and computation/data upload stages and vice versa
	GLuint frameIndex = 0;
	// Used to pace the spawning of objects at regular intervals
	GLfloat secondsSincePickup = 0.0f;
	GLfloat secondsSinceEnemy = 0.0f;

#pragma region "object_initialization"
	// Setup box VAO
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BOX_VERTICES), &BOX_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup floor VAO
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOOR_VERTICES), &FLOOR_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup wall VAO
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WALL_VERTICES), &WALL_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), &SKYBOX_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
#pragma endregion

#pragma region "flock_initialization"
	// Allocate space for shader storage buffer objects to store and retrieve data pertaining to members of the flock; also bind those objects to the current context
	glGenBuffers(2, flockBuffers);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flockBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(Fish), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flockBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(Fish), NULL, GL_DYNAMIC_COPY);

	// Upload flock member vertex data to be used during the rendering stage
	glGenBuffers(1, &geometryVBO);
	glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FISH_VERTICES), FISH_VERTICES, GL_STATIC_DRAW);

	// Setup flock VAO
	glGenVertexArrays(2, flockVAO);

	int i;

	for (i = 0; i < 2; i++)
	{
		// Bind the geometry data of a flock member to the shaders
		glBindVertexArray(flockVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)(6 * sizeof(glm::vec3)));
		// Bind the storage into which the shaders can write computed data
		glBindBuffer(GL_ARRAY_BUFFER, flockBuffers[i]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Fish), NULL);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Fish), (void *)sizeof(glm::vec4));
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	// Map the entirety of a buffer object's data store into the client's address space so we can write data to it
	glBindBuffer(GL_ARRAY_BUFFER, flockBuffers[0]);
	Fish * ptr = reinterpret_cast<Fish *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, FLOCK_SIZE * sizeof(Fish), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (i = 0; i < FLOCK_SIZE; i++)
	{
		ptr[i].position = (glm::vec3(random_float(), random_float(), random_float()) - glm::vec3(0.5f)) * 300.0f;
		ptr[i].velocity = (glm::vec3(random_float(), random_float(), random_float()) - glm::vec3(0.5f));
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
#pragma endregion

	// Load textures
	_gabenGameResourceManager.loadTexture("Textures/Life of Gaben/grass.jpg", false, "floor");
	GLuint floorTexture = _gabenGameResourceManager.getTexture("floor").ID;
	_gabenGameResourceManager.loadTexture("Textures/Life of Gaben/wall.jpg", false, "wall");
	GLuint wallTexture = _gabenGameResourceManager.getTexture("wall").ID;

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("Textures/Skybox/Life of Gaben/right.jpg");
	faces.push_back("Textures/Skybox/Life of Gaben/left.jpg");
	faces.push_back("Textures/Skybox/Life of Gaben/top.jpg");
	faces.push_back("Textures/Skybox/Life of Gaben/bottom.jpg");
	faces.push_back("Textures/Skybox/Life of Gaben/back.jpg");
	faces.push_back("Textures/Skybox/Life of Gaben/front.jpg");
	_gabenGameResourceManager.loadCubemap(faces);
	GLuint cubemapTexture = _gabenGameResourceManager.getCubemap();

	// Load models
	genesis::Model house("Objects/Life of Gaben/House/Farmhouse OBJ.obj");
	genesis::Model rock("Objects/Rock/rock.obj");
	genesis::Model pickup("Objects/Life of Gaben/Pickup/cup OBJ.obj");
	genesis::Model enemy("Objects/Nanosuit/nanosuit.obj");

	// Create game objects
	genesis::GameObject3D floorObject(shader, floorTexture, floorVAO, 6);
	genesis::GameObject3D houseObject(shader, house, glm::vec3(-30.0f, -1.05f, 5.0f), glm::vec3(0.55f, 0.55f, 0.55f));
	vector<genesis::Enemy> enemyObjects;
	vector<genesis::GameObject3D> pickupObjects;
	// Little hack to ensure that the wall textures actually render - not sure why this works!
	pickupObjects.push_back(genesis::GameObject3D(shader, pickup, glm::vec3(20.0f, 0.0f, 30.0f), glm::vec3(0.025f, 0.025f, 0.025f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
	vector<genesis::GameObject3D> wallObjects;
	GLfloat west = -12.f, east = 14.f, south = 25.f, north = -10.f;
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(west, 0.0f, 0.0f)));
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(east, 0.0f, 0.0f)));
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(0.0f, 0.0f, south), glm::vec3(1.0f), 90.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(0.0f, 0.0f, north), glm::vec3(1.0f), 90.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	vector<genesis::GameObject3D> rockObjects;
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(0.0f, -0.95f, 19.0f), glm::vec3(0.25f, 0.25f, 0.25f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(0.0f, -0.95f, 8.0f), glm::vec3(0.25f, 0.25f, 0.25f), 45.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(-4.5f, -0.95f, 13.0f), glm::vec3(0.25f, 0.25f, 0.25f), 90.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(5.0f, -0.95f, 13.0f), glm::vec3(0.25f, 0.25f, 0.25f), 135.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(-3.0f, -0.95f, 10.0f), glm::vec3(0.25f, 0.25f, 0.25f), 180.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(3.5f, -0.95f, 10.0f), glm::vec3(0.25f, 0.25f, 0.25f), 235.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(-3.0f, -0.95f, 17.0f), glm::vec3(0.25f, 0.25f, 0.25f), 22.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(3.5f, -0.95f, 17.0f), glm::vec3(0.25f, 0.25f, 0.25f), 67.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
	for (genesis::GameObject3D &rockObject : rockObjects)
	{
		rockObject._hitboxRadius = 0.46f;
		rockObject._hitboxOffset = glm::vec3(0.0f, -0.1f, 0.0f);
	}
	vector<genesis::GameObject3D> boxObjects;
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(-11.0f, 0.0f, -9.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(-11.0f, 0.0f, 24.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(13.0f, 0.0f, 24.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(13.0f, 0.0f, -9.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(6.0f, 0.0f, -3.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(-5.0f, 0.0f, -2.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(0.0f, 0.0f, -7.0f)));
	for (genesis::GameObject3D &boxObject : boxObjects)
	{
		boxObject._hitboxRadius = 1.5f;
		boxObject._hitboxOffset = glm::vec3(0.0f);
	}

	glEnable(GL_DEPTH_TEST);

	// Play theme song
	_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/theme.mp3", GL_TRUE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_gabenGameInputManager.setDeltaTime(currentFrame - _gabenGameInputManager.getLastFrame());
		_gabenGameInputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_gabenGameInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Slowly decays the movement speed of Gaben if greater than base speed
		if (_gabenGameInputManager._camera.MovementSpeed > 3.0f)
			_gabenGameInputManager._camera.MovementSpeed -= _gabenGameInputManager.getDeltaTime() / 7.0f;

		// Draw skybox first
		glDepthMask(GL_FALSE); // Remember to turn depth writing off

		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(_gabenGameInputManager._camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(_gabenGameInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(uniforms.skybox.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniforms.skybox.projection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE); // Turn depth writing back on

		// Then draw scene as normal
		shader.Use();
		view = _gabenGameInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.object.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniforms.object.projection, 1, GL_FALSE, glm::value_ptr(projection));

		// Floor
		floorObject.render();
		// House
		houseObject.render();
		// Enemy Spawns
		secondsSinceEnemy += _gabenGameInputManager.getDeltaTime();
		if (secondsSinceEnemy >= 10.0f)
		{
			secondsSinceEnemy = 0.0f;
			GLfloat x_rand = random_range(west + 2, east - 2);
			GLfloat z_rand = random_range(0.0f, south - 2);
			enemyObjects.push_back(genesis::Enemy(shader, enemy, glm::vec3(x_rand, -0.2f, z_rand), glm::vec3(0.10f, 0.10f, 0.10f)));
			enemyObjects.back()._hitboxRadius = 0.4f;
			enemyObjects.back()._hitboxOffset = glm::vec3(0.0f, 0.4f, 0.0f);
			enemyObjects.back().setAggroRadius(5.0f);
			enemyObjects.back().setDamageRadius(2.0f);
		}
		for (genesis::Enemy &enemyObject : enemyObjects)
		{
			enemyObject._translation.y = -0.2f + sinf(currentFrame) / 4;
			enemyObject.render();
			resolveCollision(enemyObject, _gabenGameInputManager);
			resolveEnemyInteractions(enemyObject, _gabenGameInputManager, _gabenGameInputManager.getDeltaTime(), DAMAGE);
		}
		// Print the health of the player
		std::cout << _health << std::endl;
		// Pickup Spawns
		secondsSincePickup += _gabenGameInputManager.getDeltaTime();
		if (secondsSincePickup >= 30.0f)
		{
			secondsSincePickup = 0.0f;
			GLfloat x_rand = random_range(west + 2, east - 2);
			GLfloat z_rand = random_range(0.0f, south - 2);
			GLfloat theta_rand = random_range(0.0f, 360.0f);
			pickupObjects.push_back(genesis::GameObject3D(shader, pickup, glm::vec3(x_rand, -0.75f, z_rand), glm::vec3(0.025f, 0.025f, 0.025f), theta_rand, glm::vec3(0.0f, 1.0f, 0.0f)));
			pickupObjects.back()._hitboxRadius = 0.22f;
			pickupObjects.back()._hitboxOffset = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		for (genesis::GameObject3D &pickupObject : pickupObjects)
		{
			if (pickupObject._rotationAngle > 360.f)
				pickupObject._rotationAngle = 0.0f;
			pickupObject._rotationAngle += _gabenGameInputManager.getDeltaTime();
			pickupObject.render();
			if (!pickupObject._destroyed && checkCollision(pickupObject, _gabenGameInputManager))
			{
				pickupObject._destroyed = true;
				_gabenGameInputManager._camera.MovementSpeed *= 2.0f;
				_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/reload.mp3", GL_FALSE);
			}
		}
		// Walls
		for (genesis::GameObject3D &wallObject : wallObjects)
		{
			wallObject.render();
		}
		resolveWallCollisions(north, south, east, west, _gabenGameInputManager);
		// Rocks
		for (genesis::GameObject3D &rockObject : rockObjects)
		{
			rockObject.render();
			resolveCollision(rockObject, _gabenGameInputManager);
		}
		// Boxes
		for (genesis::GameObject3D &boxObject : boxObjects)
		{
			boxObject.render();
			resolveCollision(boxObject, _gabenGameInputManager);
		}
#pragma region "flock_render"
		flockUpdateShader.Use();
		// Shift the flock convergence point over time to create a more dynamic simulation
		glm::vec3 goal = glm::vec3(sinf(currentFrame * 0.34f * PI_F / 180.f),
			cosf(currentFrame * 0.29f * PI_F / 180.f),
			sinf(currentFrame * 0.12f) * cosf(currentFrame * 0.5f * PI_F / 180.f));
		goal = goal * glm::vec3(35.0f, 105.0f, 60.0f);
		glUniform3fv(uniforms.flockUpdate.goal, 1, glm::value_ptr(goal));

		// Swap the flock buffers (one will be used for rendering and the other will be written 
		// into from the shader for the next frame, when it will be used for rendering)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flockBuffers[frameIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flockBuffers[frameIndex ^ 1]);

		glDispatchCompute(NUM_WORKGROUPS, 1, 1);

		flockShader.Use();
		glm::mat4 mvp = projection * view;
		glUniformMatrix4fv(uniforms.flock.mvp, 1, GL_FALSE, glm::value_ptr(mvp));

		// Render the flock
		glBindVertexArray(flockVAO[frameIndex]);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, FLOCK_SIZE);
		frameIndex ^= 1;
#pragma endregion

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

// Calculates which direction a vector is facing (N,E,S or W)
Direction vectorDirection(glm::vec2 _target)
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

/** http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/ */
glm::quat rotationBetweenVectors(glm::vec3 _start, glm::vec3 _dest) 
{
	_start = glm::normalize(_start);
	_dest = glm::normalize(_dest);

	float cosTheta = dot(_start, _dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) 
	{
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to _start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), _start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), _start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = cross(_start, _dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(s * 0.5f, rotationAxis.x * invs, 
		rotationAxis.y * invs, rotationAxis.z * invs);
}

GLboolean checkCollision(genesis::GameObject3D &_object, genesis::InputManager &_inputManager)
{
	glm::vec3 hitboxPosition = _object._translation + _object._hitboxOffset;
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	/** Collision detection booleans */
	// Collision right of the hitbox?
	bool collisionX = hitboxPosition.x + _object._hitboxRadius >= cameraPosition.x &&
		cameraPosition.x >= hitboxPosition.x;
	// Collision behind the hitbox?
	bool collisionZ = hitboxPosition.z + _object._hitboxRadius >= cameraPosition.z &&
		cameraPosition.z >= hitboxPosition.z;
	// Collision left of the hitbox?
	bool collisionX2 = hitboxPosition.x - _object._hitboxRadius <= cameraPosition.x &&
		cameraPosition.x <= hitboxPosition.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = hitboxPosition.z - _object._hitboxRadius <= cameraPosition.z &&
		cameraPosition.z <= hitboxPosition.z;

	return (collisionX && collisionZ) || (collisionX2 && collisionZ)
		|| (collisionX && collisionZ2) || (collisionX2 && collisionZ2);
}

void resolveCollision(genesis::GameObject3D &_object, genesis::InputManager &_inputManager)
{
	glm::vec3 hitboxPosition = _object._translation + _object._hitboxOffset;
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	/** Collision detection booleans */
	// Collision right of the hitbox?
	bool collisionX = hitboxPosition.x + _object._hitboxRadius >= cameraPosition.x &&
		cameraPosition.x >= hitboxPosition.x;
	// Collision behind the hitbox?
	bool collisionZ = hitboxPosition.z + _object._hitboxRadius >= cameraPosition.z &&
		cameraPosition.z >= hitboxPosition.z;
	// Collision left of the hitbox?
	bool collisionX2 = hitboxPosition.x - _object._hitboxRadius <= cameraPosition.x &&
		cameraPosition.x <= hitboxPosition.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = hitboxPosition.z - _object._hitboxRadius <= cameraPosition.z &&
		cameraPosition.z <= hitboxPosition.z;

	Direction dir = vectorDirection(glm::vec2(cameraPosition.x - hitboxPosition.x, cameraPosition.z - hitboxPosition.z));

	if (collisionX && collisionZ)
	{
		GLfloat penetrationX = hitboxPosition.x + _object._hitboxRadius - cameraPosition.x;
		GLfloat penetrationZ = hitboxPosition.z + _object._hitboxRadius - cameraPosition.z;
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x += penetrationX;
		else
			_inputManager._camera.Position.z += penetrationZ;
	}
	else if (collisionX2 && collisionZ)
	{
		GLfloat penetrationX = cameraPosition.x - (hitboxPosition.x - _object._hitboxRadius);
		GLfloat penetrationZ = hitboxPosition.z + _object._hitboxRadius - cameraPosition.z;
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x -= penetrationX;
		else
			_inputManager._camera.Position.z += penetrationZ;
	}
	else if (collisionX && collisionZ2)
	{
		GLfloat penetrationX = hitboxPosition.x + _object._hitboxRadius - cameraPosition.x;
		GLfloat penetrationZ = cameraPosition.z - (hitboxPosition.z - _object._hitboxRadius);
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x += penetrationX;
		else
			_inputManager._camera.Position.z -= penetrationZ;
	}
	else if (collisionX2 && collisionZ2)
	{
		GLfloat penetrationX = cameraPosition.x - (hitboxPosition.x - _object._hitboxRadius);
		GLfloat penetrationZ = cameraPosition.z - (hitboxPosition.z - _object._hitboxRadius);
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x -= penetrationX;
		else
			_inputManager._camera.Position.z -= penetrationZ;
	}
}

void resolveEnemyInteractions(genesis::Enemy &_object, genesis::InputManager &_inputManager, GLfloat _velocity, GLuint _damage)
{
	glm::vec3 hitboxPosition = _object._translation + _object._hitboxOffset;
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	if (_object.getIsAggroed())
	{
		// Make the enemy move towards the player
		glm::vec3 dir = _inputManager._camera.Position - _object._translation;
		dir = glm::normalize(dir);
		_object._translation += 3 * _velocity * dir;
		// Make the enemy rotate towards the player
		dir = glm::vec3(dir.x, 0.0f, dir.z);
		dir = glm::normalize(dir);
		// Find the rotation between the front of the object (that we assume towards +Z,
		// but this depends on your model) and the desired direction
		glm::quat targetOrientation = rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), dir);
		// Interpolate between start orientation and target orientation
		_object.setOrientation(glm::slerp(_object.getOrientation(), targetOrientation, _velocity));
		_object._rotationAngle = 2 * acosf(_object.getOrientation().w);

		/** Collision detection booleans */
		// Collision right of the hitbox?
		bool collisionX = hitboxPosition.x + _object.getDamageRadius() >= cameraPosition.x &&
			cameraPosition.x >= hitboxPosition.x;
		// Collision behind the hitbox?
		bool collisionZ = hitboxPosition.z + _object.getDamageRadius() >= cameraPosition.z &&
			cameraPosition.z >= hitboxPosition.z;
		// Collision left of the hitbox?
		bool collisionX2 = hitboxPosition.x - _object.getDamageRadius() <= cameraPosition.x &&
			cameraPosition.x <= hitboxPosition.x;
		// Collision in front of the hitbox?
		bool collisionZ2 = hitboxPosition.z - _object.getDamageRadius() <= cameraPosition.z &&
			cameraPosition.z <= hitboxPosition.z;

		if (_health > 0 && ((collisionX && collisionZ) || (collisionX2 && collisionZ) ||
			(collisionX && collisionZ2) || (collisionX2 && collisionZ2)))
		{
			if (_secondsSinceDamaged > 1.0f)
			{
				_secondsSinceDamaged = 0.0f;
				_health -= _damage;
				_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/hit.wav", GL_FALSE);
			}
			else
				_secondsSinceDamaged += _velocity;
		}

		return;
	}

	/** Aggro detection booleans */
	// Collision right of the hitbox?
	bool collisionX = hitboxPosition.x + _object.getAggroRadius() >= cameraPosition.x &&
		cameraPosition.x >= hitboxPosition.x;
	// Collision behind the hitbox?
	bool collisionZ = hitboxPosition.z + _object.getAggroRadius() >= cameraPosition.z &&
		cameraPosition.z >= hitboxPosition.z;
	// Collision left of the hitbox?
	bool collisionX2 = hitboxPosition.x - _object.getAggroRadius() <= cameraPosition.x &&
		cameraPosition.x <= hitboxPosition.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = hitboxPosition.z - _object.getAggroRadius() <= cameraPosition.z &&
		cameraPosition.z <= hitboxPosition.z;

	Direction dir = vectorDirection(glm::vec2(cameraPosition.x - hitboxPosition.x, cameraPosition.z - hitboxPosition.z));

	if (!_object.getIsAggroed() && ((collisionX && collisionZ) || (collisionX2 && collisionZ) || 
		(collisionX && collisionZ2) || (collisionX2 && collisionZ2)))
	{
		_object.setIsAggroed(true);
		_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/aggro.ogg", GL_FALSE);
	}
}

// Note the coordinate bounds within the game world and ensure proper collision resolution at those limits
void resolveWallCollisions(GLfloat _north, GLfloat _south, GLfloat _east, GLfloat _west, genesis::InputManager &_inputManager)
{
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	GLfloat southBoundary = _south - 0.5f, northBoundary = _north + 0.5f, westBoundary = _west + 0.5f, eastBoundary = _east - 0.5f;

	if (cameraPosition.z >= southBoundary)
	{
		GLfloat penetration = cameraPosition.z - southBoundary;
		_inputManager._camera.Position.z -= penetration;
	}
	else if (cameraPosition.z <= northBoundary)
	{
		GLfloat penetration = northBoundary - cameraPosition.z;
		_inputManager._camera.Position.z += penetration;
	}
	else if (cameraPosition.x >= eastBoundary)
	{
		GLfloat penetration = cameraPosition.x - eastBoundary;
		_inputManager._camera.Position.x -= penetration;
	}
	else if (cameraPosition.x <= westBoundary)
	{
		GLfloat penetration = westBoundary - cameraPosition.x;
		_inputManager._camera.Position.x += penetration;
	}
}