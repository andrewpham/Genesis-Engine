// GL includes
#include <gengine/Model.h>
#include "RealisticSkin.h"

genesis::InputManager _skinDemoInputManager;
genesis::ResourceManager _skinDemoResourceManager;

void run_skin_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _skinDemoResourceManager.loadShader("Shaders/Realistic Skin/sss.vs", "Shaders/Realistic Skin/sss.frag", "shader");

	struct
	{
		GLint view;
		GLint projection;
	} uniforms;

	// Cache the uniform locations
	shader.Use();
	uniforms.view = glGetUniformLocation(shader.ID, "view");
	uniforms.projection = glGetUniformLocation(shader.ID, "projection");

	// Load models
	genesis::Model head("Objects/Head 2/HeadScan_ideal.obj");

	// Create game objects
	genesis::GameObject3D headObject(shader, head, glm::vec3(-24.0f, 144.0f, -29.7f));

	// Set projection matrix
	glm::mat4 projection = glm::perspective(_skinDemoInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set view matrix
		glm::mat4 view = _skinDemoInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(view));

		// Draw the head
		headObject.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}