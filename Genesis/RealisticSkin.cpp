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
		GLint lightPos;
		GLint viewPos;
		GLint profile1On;
		GLint profile2On;
		GLint profile3On;
		GLint profile4On;
		GLint profile5On;
		GLint profile6On;
		GLint scatteringOn;
	} uniforms;

	// Cache the uniform locations
	shader.Use();
	uniforms.view = glGetUniformLocation(shader.ID, "view");
	uniforms.projection = glGetUniformLocation(shader.ID, "projection");
	uniforms.lightPos = glGetUniformLocation(shader.ID, "lightPos");
	uniforms.viewPos = glGetUniformLocation(shader.ID, "viewPos");
	uniforms.profile1On = glGetUniformLocation(shader.ID, "profile1On");
	uniforms.profile2On = glGetUniformLocation(shader.ID, "profile2On");
	uniforms.profile3On = glGetUniformLocation(shader.ID, "profile3On");
	uniforms.profile4On = glGetUniformLocation(shader.ID, "profile4On");
	uniforms.profile5On = glGetUniformLocation(shader.ID, "profile5On");
	uniforms.profile6On = glGetUniformLocation(shader.ID, "profile6On");
	uniforms.scatteringOn = glGetUniformLocation(shader.ID, "scatteringOn");

	// Used to pace the toggling of diffusion profiles
	GLfloat toggleCooldown = 0.0f;
	GLboolean p1State = false, p2State = false, p3State = false, p4State = false, p5State = false, p6State = false, hasScattering = false;

	// Load models
	genesis::Model head("Objects/Head 2/HeadScan_ideal.obj");

	// Create game objects
	genesis::GameObject3D headObject(shader, head, glm::vec3(-24.0f, 144.0f, -29.7f));

	// Set projection matrix
	glm::mat4 projection = glm::perspective(_skinDemoInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(uniforms.lightPos, LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z);

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_skinDemoInputManager.setDeltaTime(currentFrame - _skinDemoInputManager.getLastFrame());
		_skinDemoInputManager.setLastFrame(currentFrame);
		// Controls the toggle cooldown and later resets the cooldown timer when we switch diffusion profiles
		if (toggleCooldown > 0.0f)
			toggleCooldown -= _skinDemoInputManager.getDeltaTime();

		// Check and call events
		glfwPollEvents();
		_skinDemoInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (toggleCooldown <= 0.0f)
		{
			if (_skinDemoInputManager._keys[GLFW_KEY_1])
			{
				p1State = !p1State;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_2])
			{
				p2State = !p2State;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_3])
			{
				p3State = !p3State;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_4])
			{
				p4State = !p4State;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_5])
			{
				p5State = !p5State;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_6])
			{
				p6State = !p6State;
				toggleCooldown = 1.0f;
			}
			// Switches off all diffusion scattering profiles
			if (_skinDemoInputManager._keys[GLFW_KEY_SPACE])
			{
				p1State = false;
				p2State = false;
				p3State = false;
				p4State = false;
				p5State = false;
				p6State = false;
				toggleCooldown = 1.0f;
			}
			// Switches on all diffusion scattering profiles
			if (_skinDemoInputManager._keys[GLFW_KEY_ENTER])
			{
				p1State = true;
				p2State = true;
				p3State = true;
				p4State = true;
				p5State = true;
				p6State = true;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_P])
			{
				hasScattering = true;
				toggleCooldown = 1.0f;
			}
			if (_skinDemoInputManager._keys[GLFW_KEY_O])
			{
				hasScattering = false;
				toggleCooldown = 1.0f;
			}
		}

		// Set view matrix
		glm::mat4 view = _skinDemoInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(uniforms.viewPos, _skinDemoInputManager._camera.Position.x, _skinDemoInputManager._camera.Position.y, _skinDemoInputManager._camera.Position.z);
		glUniform1i(uniforms.profile1On, p1State);
		glUniform1i(uniforms.profile2On, p2State);
		glUniform1i(uniforms.profile3On, p3State);
		glUniform1i(uniforms.profile4On, p4State);
		glUniform1i(uniforms.profile5On, p5State);
		glUniform1i(uniforms.profile6On, p6State);
		glUniform1i(uniforms.scatteringOn, hasScattering);

		// Draw the head
		headObject.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}