// GL includes
#include <gengine/Model.h>
#include "RigidBodySim.h"

genesis::InputManager _physicsSimInputManager;
genesis::ResourceManager _physicsSimResourceManager;

void run_physics_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _physicsSimResourceManager.loadShader("Shaders/Rigid Body Sim/object.vs", "Shaders/Rigid Body Sim/object.frag", "shader");

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
	genesis::Model floor("Objects/Crate/Crate2.obj");
	genesis::Model box("Objects/Crate/Crate1.obj");

	// Create game objects
	genesis::GameObject3D floorObject(shader, floor, glm::vec3(-1.0f, -3.0f, -1.0f));
	vector<genesis::GameObject3D> boxObjects;
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 0.0f, 0.0f)));

	// Set projection matrix
	glm::mat4 projection = glm::perspective(_physicsSimInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_physicsSimInputManager.setDeltaTime(currentFrame - _physicsSimInputManager.getLastFrame());
		_physicsSimInputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_physicsSimInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set view matrix
		glm::mat4 view = _physicsSimInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(view));

		// Draw the floor
		floorObject.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}