// GL includes
#include <gengine/Model.h>
#include "RealisticSkin.h"

genesis::InputManager _skinDemoInputManager;
genesis::ResourceManager _skinDemoResourceManager;

void run_skin_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _skinDemoResourceManager.loadShader("Shaders/Realistic Skin/sss.vs", "Shaders/Realistic Skin/sss.frag", "shader");

	// Load models
	genesis::Model head("Objects/Head/Head.obj");

	// Set projection matrix
	glm::mat4 projection = glm::perspective(_skinDemoInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_skinDemoInputManager.setDeltaTime(currentFrame - _skinDemoInputManager.getLastFrame());
		_skinDemoInputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_skinDemoInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		// Transformation matrices
		glm::mat4 view = _skinDemoInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -165.0f, -40.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		head.Draw(shader);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}