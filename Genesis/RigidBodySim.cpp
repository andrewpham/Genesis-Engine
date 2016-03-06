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

	// Used to pace the spawning of objects at regular intervals
	GLfloat attackCooldown = 0.0f;

	// Load models
	genesis::Model floor("Objects/Crate/Crate2.obj");
	genesis::Model box("Objects/Crate/Crate1.obj");

	// Create game objects
	genesis::GameObject3D floorObject(shader, floor, glm::vec3(-1.0f, -3.0f, -1.0f));
	vector<genesis::GameObject3D> boxObjects;
	// Top left spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 0.5f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 1.0f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 1.5f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 2.0f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 2.5f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 3.0f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 3.5f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 4.0f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 4.5f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 5.0f, -1.0f)));
	// Top right spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, -0.75f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, -0.45f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, -0.15f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 0.15f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 0.45f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 0.75f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 1.05f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 1.35f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 1.65f, -1.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 1.95f, -1.0f)));
	// Bottom left spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 0.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 1.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 1.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 2.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 2.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 3.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 3.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 4.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 4.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-1.0f, 5.0f, 0.75f)));
	// Bottom right spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 0.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 1.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 1.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 2.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 2.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 3.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 3.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 4.0f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 4.5f, 0.75f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.75f, 5.0f, 0.75f)));

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
		// Controls the gunshot cooldown and later resets the cooldown timer when we fire a box
		if (attackCooldown > 0.0f)
			attackCooldown -= _physicsSimInputManager.getDeltaTime();

		// Check and call events
		glfwPollEvents();
		_physicsSimInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Spawns a box every time we fire
		if (_physicsSimInputManager._keys[GLFW_KEY_SPACE] && attackCooldown <= 0.0f)
		{
			_physicsSimInputManager.getSoundEngine()->play2D("../Genesis/Audio/Rigid Body Sim/boxspawn.wav", GL_FALSE);
			boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(_physicsSimInputManager._camera.Position.x + _physicsSimInputManager._camera.Front.x, _physicsSimInputManager._camera.Position.y + _physicsSimInputManager._camera.Front.y, _physicsSimInputManager._camera.Position.z + _physicsSimInputManager._camera.Front.z)));
			boxObjects.back().setVelocity(5.0f * _physicsSimInputManager._camera.Front);
			attackCooldown = 0.5f;
		}

		// Set view matrix
		glm::mat4 view = _physicsSimInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(view));

		// Draw the floor
		floorObject.render();
		// Draw the boxes
		for (genesis::GameObject3D &boxObject : boxObjects)
		{
			boxObject.render();
			boxObject.setVelocity(boxObject.getVelocity() + G_CONST * _physicsSimInputManager.getDeltaTime());
			boxObject.setPosition(boxObject.getPosition() + boxObject.getVelocity() * _physicsSimInputManager.getDeltaTime());
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}