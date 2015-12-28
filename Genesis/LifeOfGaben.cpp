// GL includes
#include <gengine/Model.h>
#include "LifeOfGaben.h"

genesis::InputManager _gabenGameInputManager;
genesis::ResourceManager _gabenGameResourceManager;

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
	_gabenGameResourceManager.loadTexture("Textures/container.jpg", true, "box");
	GLuint boxTexture = _gabenGameResourceManager.getTexture("box").ID;
	_gabenGameResourceManager.loadTexture("Textures/Life of Gaben/grass.jpg", true, "floor");
	GLuint floorTexture = _gabenGameResourceManager.getTexture("floor").ID;
	_gabenGameResourceManager.loadTexture("Textures/Life of Gaben/wall.jpg", true, "wall");
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
	genesis::Model obstacle("Objects/Rock/rock.obj");

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

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
		glm::mat4 model;
		view = _gabenGameInputManager._camera.GetViewMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// House
		model = glm::translate(model, glm::vec3(-30.0f, -1.05f, 5.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.55f, 0.55f, 0.55f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		house.Draw(shader);
		// Obstacle
		model = glm::mat4(); // Translate it down a bit so it's at the center of the scene
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		obstacle.Draw(shader);
		// Boxes
		glBindVertexArray(boxVAO);
		glBindTexture(GL_TEXTURE_2D, wallTexture);  // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (sampler used in fragment is set to 0 as well as default)		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-11.0f, 0.0f, -9.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-11.0f, 0.0f, 24.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(13.0f, 0.0f, 24.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(13.0f, 0.0f, -9.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, -3.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -2.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -7.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Floor
		glBindVertexArray(floorVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		// Left Wall
		glBindVertexArray(wallVAO);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-12.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		// Right Wall
		glBindVertexArray(wallVAO);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(14.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		// Back Wall
		glBindVertexArray(wallVAO);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
		model = glm::rotate(model, 90.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		// Front Wall
		glBindVertexArray(wallVAO);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
		model = glm::rotate(model, 90.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}