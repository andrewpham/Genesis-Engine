// GL includes
#include <gengine/Model.h>
#include "RealisticSkin.h"

genesis::InputManager _skinDemoInputManager;
genesis::ResourceManager _skinDemoResourceManager;

void run_skin_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _skinDemoResourceManager.loadShader("Shaders/Realistic Skin/object.vs", "Shaders/Realistic Skin/object.frag", "shader");

	// Load models
	//genesis::Model nanosuit("Objects/Nanosuit/nanosuit.obj");

	// Load texture
	_skinDemoResourceManager.loadTexture("../Genesis/Objects/Head/subdermal.png", false, "skin");
	GLuint skinTexture = _skinDemoResourceManager.getTexture("skin").ID;

	// Set projection matrix
	glm::mat4 projection = glm::perspective(_skinDemoInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	//shader.Use();
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(shader.ID, "MVP");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = genesis::loadOBJ("Objects/Head/head.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

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
		glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		// Transformation matrices
		glm::mat4 view = _skinDemoInputManager._camera.GetViewMatrix();
		//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -165.0f, -40.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// It's a bit too big for our scene, so scale it down
		glm::mat4 MVP = projection * view * model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skinTexture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(skinTexture, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}