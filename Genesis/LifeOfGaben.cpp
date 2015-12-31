// GL includes
#include <gengine/Model.h>
#include "LifeOfGaben.h"

genesis::InputManager _gabenGameInputManager;
genesis::ResourceManager _gabenGameResourceManager;

Direction vectorDirection(glm::vec2);
void resolveCollision(genesis::GameObject3D&, genesis::InputManager&);
void resolveWallCollisions(GLfloat, GLfloat, GLfloat, GLfloat, genesis::InputManager&);

void run_gaben_game(GLFWwindow* window)
{
	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup and compile our shaders
	genesis::Shader shader = _gabenGameResourceManager.loadShader("Shaders/Life of Gaben/object.vs", "Shaders/Life of Gaben/object.frag", "object");
	genesis::Shader skyboxShader = _gabenGameResourceManager.loadShader("Shaders/Life of Gaben/skybox.vs", "Shaders/Life of Gaben/skybox.frag", "skybox");

#pragma region "object_initialization"
	GLfloat boxVertices[] = {
		// Positions          // Texture Coords
		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
		1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
		1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
		1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
		1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f
	};
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	GLfloat floorVertices[] = {
		// Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		250.0f,  -1.0f,  250.0f,  250.0f, 0.0f,
		-250.0f, -1.0f,  250.0f,  0.0f, 0.0f,
		-250.0f, -1.0f, -250.0f,  0.0f, 250.0f,

		250.0f,  -1.0f,  250.0f,  250.0f, 0.0f,
		-250.0f, -1.0f, -250.0f,  0.0f, 250.0f,
		250.0f,  -1.0f, -250.0f,  250.0f, 250.0f
	};
	GLfloat wall[] = {
		// Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		0.0f,  1.0f,  40.0f,  40.0f, 0.0f,
		0.0f, 1.0f,  -40.0f,  0.0f, 0.0f,
		0.0f, -1.0f, -40.0f,  0.0f, 1.0f,

		0.0f,  1.0f,  40.0f,  40.0f, 0.0f,
		0.0f, -1.0f, -40.0f,  0.0f, 1.0f,
		0.0f,  -1.0f, 40.0f,  40.0f, 1.0f
	};
	// Setup box VAO
	GLuint boxVAO, boxVBO;
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup floor VAO
	GLuint floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup left wall VAO
	GLuint wallVAO, wallVBO;
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wall), &wall, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Load textures
	_gabenGameResourceManager.loadTexture("Textures/Life of Gaben/grass.jpg", false, "floor");
	GLuint floorTexture = _gabenGameResourceManager.getTexture("floor").ID;
	_gabenGameResourceManager.loadTexture("Textures/Life of Gaben/wall.jpg", false, "wall");
	GLuint wallTexture = _gabenGameResourceManager.getTexture("wall").ID;
#pragma endregion

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

	// Create game objects
	genesis::GameObject3D floorObject(shader, floorTexture, floorVAO, 6);
	genesis::GameObject3D houseObject(shader, house, glm::vec3(-30.0f, -1.05f, 5.0f), glm::vec3(0.55f, 0.55f, 0.55f));
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

		// Draw skybox first
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(_gabenGameInputManager._camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(_gabenGameInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		// Then draw scene as normal
		shader.Use();
		view = _gabenGameInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Floor
		floorObject.render();
		// House
		houseObject.render();
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