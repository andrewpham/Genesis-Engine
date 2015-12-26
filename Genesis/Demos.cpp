// GL includes
#include <gengine/Model.h>
#include "Demos.h"

genesis::InputManager _inputManager;
genesis::ResourceManager _resourceManager;

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

void render_superbible_perpixelgloss(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/perpixelgloss.vs", "Shaders/perpixelgloss.frag", "shader");

	GLuint tex_envmap;
	GLuint tex_glossmap;

	struct 
	{
		GLint mv_matrix;
		GLint proj_matrix;
	} uniforms;

	sb7::object object;

	shader.Use();

	uniforms.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");

	glActiveTexture(GL_TEXTURE0);
	tex_envmap = sb7::ktx::file::load("Textures/mountains3d.ktx");

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1);
	tex_glossmap = sb7::ktx::file::load("Textures/pattern1.ktx");

	object.load("sb7objects/torus_nrms_tc.sbm");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat ones[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, tex_envmap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_glossmap);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		shader.Use();

		float currentTime = glfwGetTime();
		glm::mat4 proj_matrix = glm::perspective(60.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -2.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime * 13.75f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime * 7.75f * PI_F / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime * 15.3f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader.ID);
	glDeleteTextures(1, &tex_envmap);

	glfwTerminate();
}

void render_superbible_cubemapenv(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/cubemapenv_render.vs", "Shaders/cubemapenv_render.frag", "shader");
	genesis::Shader skyboxShader = _resourceManager.loadShader("Shaders/cubemapenv_skybox.vs", "Shaders/cubemapenv_skybox.frag", "shaderShader");
	
	GLuint tex_envmap;
	GLuint envmaps[3];

	struct 
	{
		struct 
		{
			GLint mv_matrix;
			GLint proj_matrix;
		} render;
		struct 
		{
			GLint view_matrix;
		} skybox;
	} uniforms;
	
	sb7::object object;

	GLuint skybox_vao;

	shader.Use();

	uniforms.render.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.render.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");

	skyboxShader.Use();

	uniforms.skybox.view_matrix = glGetUniformLocation(skyboxShader.ID, "view_matrix");

	envmaps[0] = sb7::ktx::file::load("Textures/mountaincube.ktx");
	tex_envmap = envmaps[_inputManager.getEnvmapIndex()];

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	object.load("sb7objects/dragon.sbm");

	glGenVertexArrays(1, &skybox_vao);
	glBindVertexArray(skybox_vao);

	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		const float t = (float)glfwGetTime() * 0.1f;

		glm::mat4 proj_matrix = glm::perspective(60.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view_matrix = glm::lookAt(glm::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t)),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mv_matrix;
		mv_matrix = view_matrix * mv_matrix;
		mv_matrix = glm::rotate(mv_matrix, t, glm::vec3(1.0f, 0.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, t * 130.1f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, -4.0f, 0.0f));

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		skyboxShader.Use();
		glBindVertexArray(skybox_vao);

		glUniformMatrix4fv(uniforms.skybox.view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));

		glDisable(GL_DEPTH_TEST);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		shader.Use();

		glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glEnable(GL_DEPTH_TEST);

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader.ID);
	glDeleteTextures(3, envmaps);

	glfwTerminate();
}

void render_superbible_equirectangular(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/equirectangular.vs", "Shaders/equirectangular.frag", "shader");

	GLuint tex_envmap;
	GLuint envmaps[3];

	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
	} uniforms;

	sb7::object object;

	shader.Use();

	uniforms.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");

	envmaps[0] = sb7::ktx::file::load("Textures/equirectangularmap1.ktx");
	tex_envmap = envmaps[_inputManager.getEnvmapIndex()];
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	object.load("sb7objects/dragon.sbm");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		tex_envmap = envmaps[_inputManager.getEnvmapIndex()];
		glBindTexture(GL_TEXTURE_2D, tex_envmap);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		shader.Use();

		float currentTime = glfwGetTime();
		glm::mat4 proj_matrix = glm::perspective(60.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -15.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime * 1.1f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, -4.0f, 0.0f));

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader.ID);
	glDeleteTextures(3, envmaps);

	glfwTerminate();
}

void render_superbible_envmapsphere(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/envmapsphere.vs", "Shaders/envmapsphere.frag", "shader");

	GLuint tex_envmap;
	GLuint envmaps[3];

	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
	} uniforms;
	
	sb7::object object;

	shader.Use();

	uniforms.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");

	envmaps[0] = sb7::ktx::file::load("Textures/spheremap1.ktx");
	envmaps[1] = sb7::ktx::file::load("Textures/spheremap2.ktx");
	envmaps[2] = sb7::ktx::file::load("Textures/spheremap3.ktx");
	tex_envmap = envmaps[_inputManager.getEnvmapIndex()];

	object.load("sb7objects/dragon.sbm");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		tex_envmap = envmaps[_inputManager.getEnvmapIndex()];
		glBindTexture(GL_TEXTURE_2D, tex_envmap);
		
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		shader.Use();

		float currentTime = glfwGetTime();
		glm::mat4 proj_matrix = glm::perspective(60.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -15.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, currentTime * 1.1f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, -4.0f, 0.0f));

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader.ID);
	glDeleteTextures(3, envmaps);

	glfwTerminate();
}

void render_superbible_rimlight(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/rimlight.vs", "Shaders/rimlight.frag", "shader");

	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
		GLint rim_color;
		GLint rim_power;
	} uniforms;

	glm::mat4 mat_rotation;

	sb7::object object;
	
	shader.Use();

	uniforms.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");
	uniforms.rim_color = glGetUniformLocation(shader.ID, "rim_color");
	uniforms.rim_power = glGetUniformLocation(shader.ID, "rim_power");

	object.load("sb7objects/dragon.sbm");

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0f;
		static double total_time = 0.0f;

		double currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float f = (float)total_time;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, -5.0f, -60.0f));
		mv_matrix = glm::rotate(mv_matrix, f * 5.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		GLfloat value[] = { 0.3f, 0.3f, 0.3f };
		glUniform3fv(uniforms.rim_color, 1, value);
		glUniform1f(uniforms.rim_power, _inputManager.getRimPower());

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	object.free();
	glDeleteProgram(shader.ID);

	glfwTerminate();
}

void render_superbible_phonglighting(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader perFragmentShader = _resourceManager.loadShader("Shaders/per-fragment-phong.vs", "Shaders/per-fragment-phong.frag", "perFragmentShader");
	genesis::Shader perVertexShader = _resourceManager.loadShader("Shaders/per-vertex-phong.vs", "Shaders/per-vertex-phong.frag", "perVertexShader");

	struct
	{
		GLuint color;
		GLuint normals;
	} textures;

	struct uniforms_block
	{
		glm::mat4 mv_matrix;
		glm::mat4 view_matrix;
		glm::mat4 proj_matrix;
	};

	GLuint uniforms_buffer;

	struct
	{
		GLint diffuse_albedo;
		GLint specular_albedo;
		GLint specular_power;
	} uniforms[2];

	sb7::object object;

	perFragmentShader.Use();

	uniforms[0].diffuse_albedo = glGetUniformLocation(perFragmentShader.ID, "diffuse_albedo");
	uniforms[0].specular_albedo = glGetUniformLocation(perFragmentShader.ID, "specular_albedo");
	uniforms[0].specular_power = glGetUniformLocation(perFragmentShader.ID, "specular_power");

	perVertexShader.Use();

	uniforms[1].diffuse_albedo = glGetUniformLocation(perFragmentShader.ID, "diffuse_albedo");
	uniforms[1].specular_albedo = glGetUniformLocation(perFragmentShader.ID, "specular_albedo");
	uniforms[1].specular_power = glGetUniformLocation(perFragmentShader.ID, "specular_power");

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

	object.load("sb7objects/sphere.sbm");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };

		if (_inputManager.getPerVertex())
			perVertexShader.Use();
		else
			perFragmentShader.Use();
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glm::vec3 view_position = glm::vec3(0.0, 0.0, 50.0f);
		glm::mat4 view_matrix = glm::lookAt(view_position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 light_position = glm::vec3(20.0f, 20.0f, 0.0f);

		glm::mat4 light_proj_matrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
		glm::mat4 light_view_matrix = glm::lookAt(light_position,
			glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

#if defined(MANY_OBJECTS)
		int i, j;

		for (j = 0; j < 7; j++)
		{
			for (i = 0; i < 7; i++)
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
				uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
					0,
					sizeof(uniforms_block),
					GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

				glm::mat4 model_matrix;
				model_matrix = glm::translate(model_matrix, glm::vec3((float)i * 2.75f - 8.25f, 6.75f - (float)j * 2.25f, 0.0f));

				block->mv_matrix = view_matrix * model_matrix;
				block->view_matrix = view_matrix;
				block->proj_matrix = glm::perspective(50.0f,
					(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
					0.1f,
					1000.0f);

				glUnmapBuffer(GL_UNIFORM_BUFFER);

				GLfloat value[] = { (float)i / 9.0f + 1.0f / 9.0f, (float)i / 9.0f + 1.0f / 9.0f, (float)i / 9.0f + 1.0f / 9.0f };
				glUniform1f(uniforms[_inputManager.getPerVertex() ? 1 : 0].specular_power, powf(2.0f, (float)j + 2.0f));
				glUniform3fv(uniforms[_inputManager.getPerVertex() ? 1 : 0].specular_albedo, 1, value);

				object.render();
			}
		}

#else
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
		uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
			0,
			sizeof(uniforms_block),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		glm::mat4 model_matrix;
		glm::scale(model_matrix, glm::vec3(7.0f));

		block->mv_matrix = view_matrix * model_matrix;
		block->view_matrix = view_matrix;
		block->proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		GLfloat value[] = { 1.0f, 1.0f, 1.0f };
		glUniform1f(uniforms[_inputManager.getPerVertex() ? 1 : 0].specular_power, 30.0f);
		glUniform3fv(uniforms[_inputManager.getPerVertex() ? 1 : 0].specular_albedo, 1, value);

		object.render();
#endif

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_csflocking(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/csflocking.vs", "Shaders/csflocking.frag", "shader");
	genesis::Shader updateShader = _resourceManager.loadShader("Shaders/csflocking.comp", "updateShader");

	GLuint flock_buffer[2];

	GLuint flock_render_vao[2];
	GLuint geometry_buffer;

	struct flock_member
	{
		glm::vec3 position;
		unsigned int : 32;
		glm::vec3 velocity;
		unsigned int : 32;
	};

	struct
	{
		struct
		{
			GLint goal;
		} update;
		struct
		{
			GLuint mvp;
		} render;
	} uniforms;

	GLuint frame_index = 0;

	updateShader.Use();

	uniforms.update.goal = glGetUniformLocation(updateShader.ID, "goal");

	shader.Use();

	uniforms.render.mvp = glGetUniformLocation(shader.ID, "mvp");

	static const glm::vec3 geometry[] =
	{
		// Positions
		glm::vec3(-5.0f, 1.0f, 0.0f),
		glm::vec3(-1.0f, 1.5f, 0.0f),
		glm::vec3(-1.0f, 1.5f, 7.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(1.0f, 1.5f, 0.0f),
		glm::vec3(1.0f, 1.5f, 7.0f),
		glm::vec3(5.0f, 1.0f, 0.0f),

		// Normals
		glm::vec3(0.0f),
		glm::vec3(0.0f),
		glm::vec3(0.107f, -0.859f, 0.00f),
		glm::vec3(0.832f, 0.554f, 0.00f),
		glm::vec3(-0.59f, -0.395f, 0.00f),
		glm::vec3(-0.832f, 0.554f, 0.00f),
		glm::vec3(0.295f, -0.196f, 0.00f),
		glm::vec3(0.124f, 0.992f, 0.00f),
	};

	glGenBuffers(2, flock_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), NULL, GL_DYNAMIC_COPY);

	int i;

	glGenBuffers(1, &geometry_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

	glGenVertexArrays(2, flock_render_vao);

	for (i = 0; i < 2; i++)
	{
		glBindVertexArray(flock_render_vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)(8 * sizeof(glm::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[i]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), NULL);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), (void *)sizeof(glm::vec4));
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[0]);
	flock_member * ptr = reinterpret_cast<flock_member *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, FLOCK_SIZE * sizeof(flock_member), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (i = 0; i < FLOCK_SIZE; i++)
	{
		ptr[i].position = (glm::vec3(random_float(), random_float(), random_float()) - glm::vec3(0.5f)) * 300.0f;
		ptr[i].velocity = (glm::vec3(random_float(), random_float(), random_float()) - glm::vec3(0.5f));
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		float t = (float)glfwGetTime();
		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const float one = 1.0f;

		updateShader.Use();

		glm::vec3 goal = glm::vec3(sinf(t * 0.34f),
			cosf(t * 0.29f),
			sinf(t * 0.12f) * cosf(t * 0.5f));

		goal = goal * glm::vec3(35.0f, 25.0f, 60.0f);

		glUniform3fv(uniforms.update.goal, 1, glm::value_ptr(goal));

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flock_buffer[frame_index]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flock_buffer[frame_index ^ 1]);

		glDispatchCompute(NUM_WORKGROUPS, 1, 1);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_WIDTH);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 mv_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -170.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0));

		glm::mat4 proj_matrix = glm::perspective(60.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			3000.0f);

		glm::mat4 mvp = proj_matrix * mv_matrix;

		glUniformMatrix4fv(uniforms.render.mvp, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(flock_render_vao[frame_index]);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, FLOCK_SIZE);

		frame_index ^= 1;

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_shapedpoints(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/shapedpoints.vs", "Shaders/shapedpoints.frag", "shader");

	GLuint VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, one);

		shader.Use();

		glPointSize(200.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_hdrtonemap(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shaderNaive = _resourceManager.loadShader("Shaders/tonemap.vs", "Shaders/tonemap_naive.frag", "shaderNaive");
	genesis::Shader shaderExposure = _resourceManager.loadShader("Shaders/tonemap.vs", "Shaders/tonemap_exposure.frag", "shaderExposure");
	genesis::Shader shaderAdaptive = _resourceManager.loadShader("Shaders/tonemap.vs", "Shaders/tonemap_adaptive.frag", "shaderAdaptive");

	GLuint tex_src;
	GLuint tex_lut;

	GLuint VAO;

	struct
	{
		struct
		{
			int exposure;
		} exposure;
	} uniforms;

	shaderExposure.Use();

	uniforms.exposure.exposure = glGetUniformLocation(shaderExposure.ID, "exposure");

	// Load texture from file
	tex_src = sb7::ktx::file::load("Textures/treelights_2k.ktx");

	// Now bind it to the context using the GL_TEXTURE_2D binding point
	glBindTexture(GL_TEXTURE_2D, tex_src);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };

	glGenTextures(1, &tex_lut);
	glBindTexture(GL_TEXTURE_1D, tex_lut);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		glClearBufferfv(GL_COLOR, 0, black);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, tex_lut);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_src);

		switch (_inputManager.getModeNo())
		{
		case 0:
			shaderNaive.Use();
			break;
		case 1:
			shaderExposure.Use();
			glUniform1f(uniforms.exposure.exposure, _inputManager.getExposure());
			break;
		case 2:
			shaderAdaptive.Use();
			break;
		}
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shaderNaive.ID);
	glDeleteProgram(shaderExposure.ID);
	glDeleteProgram(shaderAdaptive.ID);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &tex_src);
	glDeleteTextures(1, &tex_lut);

	glfwTerminate();
}

void render_superbible_polygonsmooth(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/linesmooth.vs", "Shaders/linesmooth.frag", "shader");

	GLuint VAO;
	GLuint position_buffer;
	GLuint index_buffer;
	GLint mv_location;
	GLint proj_location;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader.ID, "proj_matrix");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_positions[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_positions),
		vertex_positions,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(vertex_indices),
		vertex_indices,
		GL_STATIC_DRAW);

	glEnable(GL_CULL_FACE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		glClearBufferfv(GL_COLOR, 0, black);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POLYGON_SMOOTH);

		float currentTime = glfwGetTime();
		float f = (float)currentTime * 0.3f;
		currentTime = 3.15;
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -4.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 45.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 81.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.ID);
	glDeleteBuffers(1, &position_buffer);

	glfwTerminate();
}

void render_superbible_linesmooth(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/linesmooth.vs", "Shaders/linesmooth.frag", "shader");

	GLuint VAO;
	GLuint position_buffer;
	GLuint index_buffer;
	GLint mv_location;
	GLint proj_location;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader.ID, "proj_matrix");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_positions[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_positions),
		vertex_positions,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(vertex_indices),
		vertex_indices,
		GL_STATIC_DRAW);

	glEnable(GL_CULL_FACE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		glClearBufferfv(GL_COLOR, 0, black);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);

		float currentTime = glfwGetTime();
		float f = (float)currentTime * 0.3f;
		currentTime = 3.15;
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -4.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 45.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 81.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.ID);
	glDeleteBuffers(1, &position_buffer);

	glfwTerminate();
}

void render_superbible_basicfbo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader1 = _resourceManager.loadShader("Shaders/basicfbo.vs", "Shaders/basicfbo.frag", "shader1");
	genesis::Shader shader2 = _resourceManager.loadShader("Shaders/basicfbo.vs", "Shaders/basicfbo2.frag", "shader2");

	GLuint VAO;
	GLuint position_buffer;
	GLuint index_buffer;
	GLuint FBO;
	GLuint color_texture;
	GLuint depth_texture;
	GLint mv_location;
	GLint proj_location;
	GLuint mv_location2;
	GLuint proj_location2;

	shader1.Use();

	mv_location = glGetUniformLocation(shader1.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader1.ID, "proj_matrix");

	shader2.Use();

	mv_location2 = glGetUniformLocation(shader2.ID, "mv_matrix");
	proj_location2 = glGetUniformLocation(shader2.ID, "proj_matrix");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_data[] =
	{
		// Position                 Tex Coord
		-0.25f, -0.25f,  0.25f,      0.0f, 1.0f,
		-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,

		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,
		0.25f, -0.25f,  0.25f,      1.0f, 1.0f,
		-0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

		0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
		0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

		0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,
		0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

		0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
		-0.25f,  0.25f, -0.25f,      0.0f, 0.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

		-0.25f,  0.25f, -0.25f,      0.0f, 0.0f,
		-0.25f,  0.25f,  0.25f,      0.0f, 1.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

		-0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
		-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		-0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

		-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		-0.25f, -0.25f,  0.25f,      0.0f, 1.0f,
		-0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

		-0.25f,  0.25f, -0.25f,      0.0f, 1.0f,
		0.25f,  0.25f, -0.25f,      1.0f, 1.0f,
		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,

		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,
		-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		-0.25f,  0.25f, -0.25f,      0.0f, 1.0f,

		-0.25f, -0.25f,  0.25f,      0.0f, 0.0f,
		0.25f, -0.25f,  0.25f,      1.0f, 0.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,
		-0.25f,  0.25f,  0.25f,      0.0f, 1.0f,
		-0.25f, -0.25f,  0.25f,      0.0f, 0.0f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &color_texture);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 512, 512);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_DEPTH_COMPONENT32F, 512, 512);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		static const GLfloat blue[] = { 0.0f, 0.0f, 0.3f, 1.0f };
		static const GLfloat one = 1.0f;

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);

		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime * 0.3f;
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -12.0f));
		mv_matrix = glm::translate(mv_matrix, glm::vec3(sinf(2.1f * f) * 0.5f,
			cosf(1.7f * f) * 0.5f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 45.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 81.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glViewport(0, 0, 512, 512);
		static const GLfloat green[] = { 0.0f, 0.3f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		shader1.Use();

		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, blue);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glBindTexture(GL_TEXTURE_2D, color_texture);

		shader2.Use();

		glUniformMatrix4fv(proj_location2, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(mv_location2, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindTexture(GL_TEXTURE_2D, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader1.ID);
	glDeleteProgram(shader2.ID);
	glDeleteBuffers(1, &position_buffer);
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &color_texture);

	glfwTerminate();
}

void render_superbible_depthclamp(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/depthclamp.vs", "Shaders/depthclamp.frag", "shader");

	GLint mv_location;
	GLint proj_location;
	GLint explode_factor_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader.ID, "proj_matrix");
	explode_factor_location = glGetUniformLocation(shader.ID, "explode_factor");

	object.load("sb7objects/dragon.sbm");

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			1.8f,
			1000.0f);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glEnable(GL_DEPTH_CLAMP);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -20.0f));
		mv_matrix = glm::rotate(mv_matrix, f * 45.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, f * 81.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		glUniform1f(explode_factor_location, sinf((float)currentTime * 3.0f) * cosf((float)currentTime * 4.0f) * 0.7f + 1.1f);

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	object.free();
	glDeleteProgram(shader.ID);

	glfwTerminate();
}

void render_superbible_multiscissor(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/multiscissor.vs", "Shaders/multiscissor.frag", "Shaders/multiscissor.gs", "shader");

	GLuint          VAO;
	GLuint          position_buffer;
	GLuint          index_buffer;
	GLuint          uniform_buffer;
	GLint           mv_location;
	GLint           proj_location;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader.ID, "proj_matrix");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_positions[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_positions),
		vertex_positions,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(vertex_indices),
		vertex_indices,
		GL_STATIC_DRAW);

	glGenBuffers(1, &uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		int i;
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		glDisable(GL_SCISSOR_TEST);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		// Turn on scissor testing
		glEnable(GL_SCISSOR_TEST);

		// Each rectangle will be 7/16 of the screen
		int scissor_width = (7 * SCREEN_WIDTH) / 16;
		int scissor_height = (7 * SCREEN_HEIGHT) / 16;

		// Four rectangles - lower left first...
		glScissorIndexed(0,
			0, 0,
			scissor_width, scissor_height);

		// Lower right...
		glScissorIndexed(1,
			SCREEN_WIDTH - scissor_width, 0,
			scissor_width, scissor_height);

		// Upper left...
		glScissorIndexed(2,
			0, SCREEN_HEIGHT - scissor_height,
			scissor_width, scissor_height);

		// Upper right...
		glScissorIndexed(3,
			SCREEN_WIDTH - scissor_width,
			SCREEN_HEIGHT - scissor_height,
			scissor_width, scissor_height);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);

		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime * 0.3f;

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);
		glm::mat4 * mv_matrix_array = (glm::mat4 *)glMapBufferRange(GL_UNIFORM_BUFFER,
			0,
			4 * sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		glm::mat4 mv_matrix;
		for (i = 0; i < 4; i++)
		{
			mv_matrix = glm::mat4();
			mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -10.0f));
			mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 45.0f * (float)(i + 1) * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 81.0f * (float)(i + 1) * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			mv_matrix_array[i] = proj_matrix * mv_matrix;
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.ID);
	glDeleteBuffers(1, &position_buffer);

	glfwTerminate();
}

void render_superbible_noperspective(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/noperspective.vs", "Shaders/noperspective.frag", "shader");

	GLuint VAO;
	GLuint tex_checker;

	struct
	{
		GLint mvp;
		GLint use_perspective;
	} uniforms;

	shader.Use();

	uniforms.mvp = glGetUniformLocation(shader.ID, "mvp");
	uniforms.use_perspective = glGetUniformLocation(shader.ID, "use_perspective");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const unsigned char checker_data[] =
	{
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	};

	glGenTextures(1, &tex_checker);
	glBindTexture(GL_TEXTURE_2D, tex_checker);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 8, 8);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, checker_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time * 14.3f;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -0.4f));
		mv_matrix = glm::rotate(mv_matrix, t * PI_F / 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 proj_matrix = glm::perspective(60.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f, 1000.0f);

		shader.Use();

		glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
		glUniform1i(uniforms.use_perspective, _inputManager.getUsePerspective());

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.ID);

	glfwTerminate();
}

void render_superbible_multiviewport(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/multiviewport.vs", "Shaders/multiviewport.frag", "Shaders/multiviewport.gs", "shader");

	GLuint VAO;
	GLuint position_buffer;
	GLuint index_buffer;
	GLuint uniform_buffer;
	GLint mv_location;
	GLint proj_location;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_positions[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	glGenBuffers(1, &uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		int i;
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		// Each rectangle will be 7/16 of the screen
		float viewport_width = (float)(7 * SCREEN_WIDTH) / 16.0f;
		float viewport_height = (float)(7 * SCREEN_HEIGHT) / 16.0f;

		// Four rectangles - lower left first...
		glViewportIndexedf(0, 0, 0, viewport_width, viewport_height);

		// Lower right...
		glViewportIndexedf(1,
			SCREEN_WIDTH - viewport_width, 0,
			viewport_width, viewport_height);

		// Upper left...
		glViewportIndexedf(2,
			0, SCREEN_HEIGHT - viewport_height,
			viewport_width, viewport_height);

		// Upper right...
		glViewportIndexedf(3,
			SCREEN_WIDTH - viewport_width,
			SCREEN_HEIGHT - viewport_height,
			viewport_width, viewport_height);

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);

		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime * 0.3f;

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);
		glm::mat4 * mv_matrix_array = (glm::mat4 *)glMapBufferRange(GL_UNIFORM_BUFFER,
			0,
			4 * sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		glm::mat4 mv_matrix;
		for (i = 0; i < 4; i++)
		{
			mv_matrix = glm::mat4();
			mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -10.0f));
			mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 45.0f * (float)(i + 1) * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 81.0f * (float)(i + 1) * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			mv_matrix_array[i] = proj_matrix * mv_matrix;
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		shader.Use();

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.ID);
	glDeleteBuffers(1, &position_buffer);

	glfwTerminate();
}

void render_superbible_gsquads(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shaderFans = _resourceManager.loadShader("Shaders/quadsasfans.vs", "Shaders/quadsasfans.frag", "shaderFans");
	genesis::Shader shaderLinesAdj = _resourceManager.loadShader("Shaders/quadsaslinesadj.vs", "Shaders/quadsaslinesadj.frag", "Shaders/quadsaslinesadj.gs", "shaderLinesAdj");

	GLuint VAO;
	int mvp_loc_fans;
	int mvp_loc_linesadj;
	int vid_offset_loc_fans;
	int vid_offset_loc_linesadj;

	shaderFans.Use();

	mvp_loc_fans = glGetUniformLocation(shaderFans.ID, "mvp");
	vid_offset_loc_fans = glGetUniformLocation(shaderFans.ID, "vid_offset");

	shaderLinesAdj.Use();

	mvp_loc_linesadj = glGetUniformLocation(shaderLinesAdj.ID, "mvp");
	vid_offset_loc_linesadj = glGetUniformLocation(shaderLinesAdj.ID, "vid_offset");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.25f, 0.0f, 1.0f };
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time;

		glClearBufferfv(GL_COLOR, 0, black);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -10.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)t * 5.0f * PI_F / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)t * 30.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 proj_matrix = glm::perspective(50.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 mvp = proj_matrix * mv_matrix;

		switch (_inputManager.getModeNo())
		{
		case 0:
			shaderFans.Use();
			glUniformMatrix4fv(mvp_loc_fans, 1, GL_FALSE, glm::value_ptr(mvp));
			glUniform1i(vid_offset_loc_fans, _inputManager.getVidOffset());
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			break;
		case 1:
			shaderLinesAdj.Use();
			glUniformMatrix4fv(mvp_loc_linesadj, 1, GL_FALSE, glm::value_ptr(mvp));
			glUniform1i(vid_offset_loc_linesadj, _inputManager.getVidOffset());
			glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
			break;
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shaderFans.ID);
	glDeleteProgram(shaderLinesAdj.ID);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
}

void render_superbible_normalviewer(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/normalviewer.vs", "Shaders/normalviewer.frag", "Shaders/normalviewer.gs", "shader");

	GLint mv_location;
	GLint proj_location;
	GLint normal_length_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader.ID, "proj_matrix");
	normal_length_location = glGetUniformLocation(shader.ID, "normal_length");

	object.load("sb7objects/torus.sbm");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -10.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 15.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 21.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		glUniform1f(normal_length_location, sinf((float)currentTime * 8.0f) * cosf((float)currentTime * 6.0f) * 0.03f + 0.05f);

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_gstessellate(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/gstessellate.vs", "Shaders/gstessellate.frag", "Shaders/gstessellate.gs", "shader");

	GLint mv_location;
	GLint mvp_location;
	GLint stretch_location;
	GLuint vao;
	GLuint buffer;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mvMatrix");
	mvp_location = glGetUniformLocation(shader.ID, "mvpMatrix");
	stretch_location = glGetUniformLocation(shader.ID, "stretch");

	static const GLfloat tetrahedron_verts[] =
	{
		0.000f,  0.000f,  1.000f,
		0.943f,  0.000f, -0.333f,
		-0.471f,  0.816f, -0.333f,
		-0.471f, -0.816f, -0.333f
	};

	static const GLushort tetrahedron_indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 1,
		3, 2, 1
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedron_verts) + sizeof(tetrahedron_indices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(tetrahedron_indices), tetrahedron_indices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedron_indices), sizeof(tetrahedron_verts), tetrahedron_verts);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(tetrahedron_indices));
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -13.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 71.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 10.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		glUniform1f(stretch_location, sinf(f * 4.0f) * 0.75f + 1.0f);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, NULL);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_objectexploder(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/objectexploder.vs", "Shaders/objectexploder.frag", "Shaders/objectexploder.gs", "shader");

	GLint mv_location;
	GLint proj_location;
	GLint explode_factor_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mv_matrix");
	proj_location = glGetUniformLocation(shader.ID, "proj_matrix");
	explode_factor_location = glGetUniformLocation(shader.ID, "explode_factor");

	object.load("sb7objects/torus.sbm");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -15.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 45.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 81.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		glUniform1f(explode_factor_location, sinf((float)currentTime * 8.0f) * cosf((float)currentTime * 6.0f) * 0.7f + 0.1f);

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_gsculling(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/gsculling.vs", "Shaders/gsculling.frag", "Shaders/gsculling.gs", "shader");

	GLint mv_location;
	GLint mvp_location;
	GLint viewpoint_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.ID, "mvMatrix");
	mvp_location = glGetUniformLocation(shader.ID, "mvpMatrix");
	viewpoint_location = glGetUniformLocation(shader.ID, "viewpoint");

	object.load("sb7objects/dragon.sbm");

	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		GLfloat currentTime = glfwGetTime();
		float f = (float)currentTime;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			0.1f,
			1000.0f);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -60.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 5.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)currentTime * 100.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		GLfloat vViewpoint[] = { sinf(f * 2.1f) * 70.0f, cosf(f * 1.4f) * 70.0f, sinf(f * 0.7f) * 70.0f };
		glUniform3fv(viewpoint_location, 1, vViewpoint);

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	object.free();
	glDeleteProgram(shader.ID);

	glfwTerminate();
}

void render_superbible_cubicbezier(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader tess_shader = _resourceManager.loadShader("Shaders/cubicbezier.vs", "Shaders/cubicbezier.frag", "Shaders/cubicbezier.tcs", "Shaders/cubicbezier.tes", "tess_shader");
	genesis::Shader draw_cp_shader = _resourceManager.loadShader("Shaders/draw-control-points.vs", "Shaders/draw-control-points.frag", "draw_cp_shader");

	GLuint patch_vao;
	GLuint patch_buffer;
	GLuint cage_indices;
	glm::vec3 patch_data[16];

	struct
	{
		struct
		{
			int mv_matrix;
			int proj_matrix;
			int mvp;
		} patch;
		struct
		{
			int draw_color;
			int mvp;
		} control_point;
	} uniforms;

	tess_shader.Use();

	uniforms.patch.mv_matrix = glGetUniformLocation(tess_shader.ID, "mv_matrix");
	uniforms.patch.proj_matrix = glGetUniformLocation(tess_shader.ID, "proj_matrix");
	uniforms.patch.mvp = glGetUniformLocation(tess_shader.ID, "mvp");

	draw_cp_shader.Use();

	uniforms.control_point.draw_color = glGetUniformLocation(draw_cp_shader.ID, "draw_color");
	uniforms.control_point.mvp = glGetUniformLocation(draw_cp_shader.ID, "mvp");

	glGenVertexArrays(1, &patch_vao);
	glBindVertexArray(patch_vao);

	glGenBuffers(1, &patch_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, patch_buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(patch_data), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	static const GLushort indices[] =
	{
		0, 1, 1, 2, 2, 3,
		4, 5, 5, 6, 6, 7,
		8, 9, 9, 10, 10, 11,
		12, 13, 13, 14, 14, 15,

		0, 4, 4, 8, 8, 12,
		1, 5, 5, 9, 9, 13,
		2, 6, 6, 10, 10, 14,
		3, 7, 7, 11, 11, 15
	};

	glGenBuffers(1, &cage_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cage_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat one = 1.0f;

		int i;
		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time;

		static const float patch_initializer[] =
		{
			-1.0f,  -1.0f,  0.0f,
			-0.33f, -1.0f,  0.0f,
			0.33f, -1.0f,  0.0f,
			1.0f,  -1.0f,  0.0f,

			-1.0f,  -0.33f, 0.0f,
			-0.33f, -0.33f, 0.0f,
			0.33f, -0.33f, 0.0f,
			1.0f,  -0.33f, 0.0f,

			-1.0f,   0.33f, 0.0f,
			-0.33f,  0.33f, 0.0f,
			0.33f,  0.33f, 0.0f,
			1.0f,   0.33f, 0.0f,

			-1.0f,   1.0f,  0.0f,
			-0.33f,  1.0f,  0.0f,
			0.33f,  1.0f,  0.0f,
			1.0f,   1.0f,  0.0f,
		};

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glEnable(GL_DEPTH_TEST);

		glm::vec3 * p = (glm::vec3 *)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(patch_buffer), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(p, patch_initializer, sizeof(patch_initializer));

		for (i = 0; i < 16; i++)
		{
			float fi = (float)i / 16.0f;
			p[i][2] = sinf(t * (0.2f + fi * 0.3f));
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		glBindVertexArray(patch_vao);

		tess_shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			1.0f, 1000.0f);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -14.0f));
		mv_matrix = glm::rotate(mv_matrix, t * 10.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, t * 17.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniforms.patch.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.patch.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.patch.mvp, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));

		if (_inputManager.getWireframe())
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, 16);

		draw_cp_shader.Use();
		glUniformMatrix4fv(uniforms.control_point.mvp, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));

		if (_inputManager.getShowPoints())
		{
			glPointSize(9.0f);
			glUniform4fv(uniforms.control_point.draw_color, 1, glm::value_ptr(glm::vec4(0.2f, 0.7f, 0.9f, 1.0f)));
			glDrawArrays(GL_POINTS, 0, 16);
		}

		if (_inputManager.getShowCage())
		{
			glUniform4fv(uniforms.control_point.draw_color, 1, glm::value_ptr(glm::vec4(0.7f, 0.9f, 0.2f, 1.0f)));
			glDrawElements(GL_LINES, 48, GL_UNSIGNED_SHORT, NULL);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_dispmap(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/dispmap.vs", "Shaders/dispmap.frag", "Shaders/dispmap.tcs", "Shaders/dispmap.tes", "shader");

	GLuint VAO;
	GLuint tex_displacement;
	GLuint tex_color;

	struct
	{
		GLint mvp_matrix;
		GLint mv_matrix;
		GLint proj_matrix;
		GLint dmap_depth;
		GLint enable_fog;
	} uniforms;

	shader.Use();

	uniforms.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.mvp_matrix = glGetUniformLocation(shader.ID, "mvp_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");
	uniforms.dmap_depth = glGetUniformLocation(shader.ID, "dmap_depth");
	uniforms.enable_fog = glGetUniformLocation(shader.ID, "enable_fog");
	_inputManager.setDmapDepth(6.0f);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_CULL_FACE);

	tex_displacement = sb7::ktx::file::load("Textures/terragen1.ktx");
	glActiveTexture(GL_TEXTURE1);
	tex_color = sb7::ktx::file::load("Textures/terragen_color.ktx");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time * 0.03f;
		float r = sinf(t * 5.37f) * 15.0f + 16.0f;
		float h = cosf(t * 4.79f) * 2.0f + 3.2f;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glm::mat4 mv_matrix = glm::lookAt(glm::vec3(sinf(t) * r, h, cosf(t) * r), glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		glm::mat4 proj_matrix = glm::perspective(60.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

		shader.Use();

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.mvp_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
		glUniform1f(uniforms.dmap_depth, _inputManager.getEnableDisplacement() ? _inputManager.getDmapDepth() : 0.0f);
		glUniform1i(uniforms.enable_fog, _inputManager.getEnableFog() ? 1 : 0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		if (_inputManager.getWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_PATCHES, GL_FILL);
		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.ID);

	glfwTerminate();
}

void render_superbible_tessmodes(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader0 = _resourceManager.loadShader("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_quads.tcs", "Shaders/tessmodes_quads.tes", "shader0");
	genesis::Shader shader1 = _resourceManager.loadShader("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_triangles.tcs", "Shaders/tessmodes_triangles.tes", "shader1");
	genesis::Shader shader2 = _resourceManager.loadShader("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_triangles.tcs", "Shaders/tessmodes_triangles_as_points.tes", "shader2");
	genesis::Shader shader3 = _resourceManager.loadShader("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_isolines.tcs", "Shaders/tessmodes_isolines.tes", "shader3");

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, black);

		shader1.Use();
		glDrawArrays(GL_PATCHES, 0, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_clipdistance(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/clipdistance.vs", "Shaders/clipdistance.frag", "shader");

	sb7::object object;

	struct
	{
		GLint proj_matrix;
		GLint mv_matrix;
		GLint clip_plane;
		GLint clip_sphere;
	} uniforms;

	shader.Use();
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");
	uniforms.mv_matrix = glGetUniformLocation(shader.ID, "mv_matrix");
	uniforms.clip_plane = glGetUniformLocation(shader.ID, "clip_plane");
	uniforms.clip_sphere = glGetUniformLocation(shader.ID, "clip_sphere");

	object.load("sb7objects/dragon.sbm");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat one = 1.0f;

		static double last_time = 0.0;
		static double total_time = 0.0;

		// Set frame time
		GLfloat currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float f = (float)total_time;

		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -80.0f));
		mv_matrix = glm::rotate(mv_matrix, f * 0.34f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, -4.0f, 0.0f));

		glm::mat4 plane_matrix;
		plane_matrix = glm::rotate(plane_matrix, f * 6.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		plane_matrix = glm::rotate(plane_matrix, f * 7.3f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec4 plane = plane_matrix[0];
		plane[3] = 0.0f;
		plane = glm::normalize(plane);

		glm::vec4 clip_sphere = glm::vec4(sinf(f * 0.7f) * 3.0f, cosf(f * 1.9f) * 3.0f, sinf(f * 0.1f) * 3.0f, cosf(f * 1.7f) + 2.5f);

		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniform4fv(uniforms.clip_plane, 1, glm::value_ptr(plane));
		glUniform4fv(uniforms.clip_sphere, 1, glm::value_ptr(clip_sphere));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CLIP_DISTANCE0);
		glEnable(GL_CLIP_DISTANCE1);

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_multidrawindirect(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/multidrawindirect.vs", "Shaders/multidrawindirect.frag", "shader");

	sb7::object         object;

	GLuint              indirect_draw_buffer;
	GLuint              draw_index_buffer;

	struct
	{
		GLint           time;
		GLint           view_matrix;
		GLint           proj_matrix;
		GLint           viewproj_matrix;
	} uniforms;

	shader.Use();

	uniforms.time = glGetUniformLocation(shader.ID, "time");
	uniforms.view_matrix = glGetUniformLocation(shader.ID, "view_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.ID, "proj_matrix");
	uniforms.viewproj_matrix = glGetUniformLocation(shader.ID, "viewproj_matrix");

	int i;

	object.load("sb7objects/asteroids.sbm");

	glGenBuffers(1, &indirect_draw_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), NULL, GL_STATIC_DRAW);

	DrawArraysIndirectCommand * cmd = (DrawArraysIndirectCommand *)
		glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (i = 0; i < NUM_DRAWS; i++)
	{
		object.get_sub_object_info(i % object.get_sub_object_count(), cmd[i].first, cmd[i].count);
		cmd[i].primCount = 1;
		cmd[i].baseInstance = i;
	}

	glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

	glBindVertexArray(object.get_vao());

	glGenBuffers(1, &draw_index_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, draw_index_buffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_DRAWS * sizeof(GLuint), NULL, GL_STATIC_DRAW);

	GLuint * draw_index =
		(GLuint *)glMapBufferRange(GL_ARRAY_BUFFER,
			0,
			NUM_DRAWS * sizeof(GLuint),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (i = 0; i < NUM_DRAWS; i++)
	{
		draw_index[i] = i;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribIPointer(10, 1, GL_UNSIGNED_INT, 0, NULL);
	glVertexAttribDivisor(10, 1);
	glEnableVertexAttribArray(10);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		int j;
		static const float one = 1.0f;
		static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };


		static double last_time = 0.0;
		static double total_time = 0.0;

		// Set frame time
		GLfloat currentTime = glfwGetTime();
		if (!_inputManager.getPaused())
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = float(total_time);
		int i = int(total_time * 3.0f);

		// Clear buffers
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		const glm::mat4 view_matrix = glm::lookAt(glm::vec3(100.0f * cosf(t * 0.023f), 100.0f * cosf(t * 0.023f), 300.0f * sinf(t * 0.037f) - 600.0f),
			glm::vec3(0.0f, 0.0f, 260.0f),
			glm::normalize(glm::vec3(0.1f - cosf(t * 0.1f) * 0.3f, 1.0f, 0.0f)));
		const glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			1.0f,
			2000.0f);

		shader.Use();

		glUniform1f(uniforms.time, t);
		glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.viewproj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix * view_matrix));

		glBindVertexArray(object.get_vao());

		if (_inputManager.getMode() == MODE_MULTIDRAW)
		{
			glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, NUM_DRAWS, 0);
		}
		else if (_inputManager.getMode() == MODE_SEPARATE_DRAWS)
		{
			for (j = 0; j < NUM_DRAWS; j++)
			{
				GLuint first, count;
				object.get_sub_object_info(j % object.get_sub_object_count(), first, count);
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES,
					first,
					count,
					1, j);
			}
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_instancedattribs(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/instancedattribs.vs", "Shaders/instancedattribs.frag", "shader");

	static const GLfloat square_vertices[] =
	{
		-1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f,
		1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};

	static const GLfloat instance_colors[] =
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f
	};

	static const GLfloat instance_positions[] =
	{
		-2.0f, -2.0f, 0.0f, 0.0f,
		2.0f, -2.0f, 0.0f, 0.0f,
		2.0f,  2.0f, 0.0f, 0.0f,
		-2.0f,  2.0f, 0.0f, 0.0f
	};

	GLuint offset = 0;

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices) + sizeof(instance_colors) + sizeof(instance_positions), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_vertices), square_vertices);
	offset += sizeof(square_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_colors), instance_colors);
	offset += sizeof(instance_colors);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_positions), instance_positions);
	offset += sizeof(instance_positions);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(square_vertices));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(square_vertices) + sizeof(instance_colors)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	shader.Use();

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearBufferfv(GL_COLOR, 0, black);

		shader.Use();
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_superbible_fragmentlist(GLFWwindow* window)
{
	struct
	{
		GLuint color;
		GLuint normals;
	} textures;

	struct uniforms_block
	{
		glm::mat4 mv_matrix;
		glm::mat4 view_matrix;
		glm::mat4 proj_matrix;
	};

	GLuint uniforms_buffer;

	struct
	{
		GLint mvp;
	} uniforms;

	sb7::object object;

	GLuint fragment_buffer;
	GLuint head_pointer_image;
	GLuint atomic_counter_buffer;
	GLuint dummy_vao;

	// Setup and compile our shaders
	genesis::Shader clearShader = _resourceManager.loadShader("Shaders/clear.vs", "Shaders/clear.frag", "clearShader");
	genesis::Shader appendShader = _resourceManager.loadShader("Shaders/append.vs", "Shaders/append.frag", "appendShader");
	genesis::Shader resolveShader = _resourceManager.loadShader("Shaders/resolve.vs", "Shaders/resolve.frag", "resolveShader");

	appendShader.Use();
	uniforms.mvp = glGetUniformLocation(appendShader.ID, "mvp");

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

	object.load("sb7objects/dragon.sbm");

	glGenBuffers(1, &fragment_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, fragment_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024 * 1024 * 16, NULL, GL_DYNAMIC_COPY);

	glGenBuffers(1, &atomic_counter_buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, 4, NULL, GL_DYNAMIC_COPY);

	glGenTextures(1, &head_pointer_image);
	glBindTexture(GL_TEXTURE_2D, head_pointer_image);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 1024, 1024);

	glGenVertexArrays(1, &dummy_vao);
	glBindVertexArray(dummy_vao);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();

		// Check and call events
		glfwPollEvents();

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		clearShader.Use();
		glBindVertexArray(dummy_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		appendShader.Use();

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, glm::vec3(7.0f, 7.0f, 7.0f));
		glm::vec3 view_position = glm::vec3(cosf(currentFrame * 0.35f) * 120.0f * 2.3f, cosf(currentFrame * 0.4f) * 30.0f * 2.3f,
			sinf(currentFrame * 0.35f) * 120.0f * 2.3f);
		glm::mat4 view_matrix = glm::lookAt(view_position, glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		glm::mat4 mv_matrix = view_matrix * model_matrix;
		glm::mat4 proj_matrix = glm::perspective(50.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

		glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));

		static const unsigned int zero = 0;
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_counter_buffer);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(zero), &zero);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fragment_buffer);

		glBindImageTexture(0, head_pointer_image, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		object.render();

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		resolveShader.Use();

		glBindVertexArray(dummy_vao);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_skybox_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/advanced.vs", "Shaders/advanced.frag", "shader");
	genesis::Shader skyboxShader = _resourceManager.loadShader("Shaders/skybox.vs", "Shaders/skybox.frag", "skyboxShader");

#pragma region "object_initialization"
	GLfloat cubeVertices[] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

	// Setup cube VAO
	GLuint cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
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
	_resourceManager.loadTexture("Textures/container.jpg");
	GLuint cubeTexture = _resourceManager.getTexture();
#pragma endregion

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("Textures/Skybox/right.jpg");
	faces.push_back("Textures/Skybox/left.jpg");
	faces.push_back("Textures/Skybox/top.jpg");
	faces.push_back("Textures/Skybox/bottom.jpg");
	faces.push_back("Textures/Skybox/back.jpg");
	faces.push_back("Textures/Skybox/front.jpg");
	_resourceManager.loadCubemap(faces);
	GLuint cubemapTexture = _resourceManager.getCubemap();

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_inputManager.setDeltaTime(currentFrame - _inputManager.getLastFrame());
		_inputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw skybox first
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(_inputManager._camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(_inputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Draw cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.ID, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		// Then draw scene as normal
		shader.Use();
		glm::mat4 model;
		view = _inputManager._camera.GetViewMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.ID, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_exploding_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _resourceManager.loadShader("Shaders/geometry.vs", "Shaders/geometry.frag", "Shaders/geometry.gs", "shader");

	// Load models
	genesis::Model katarina("Objects/Katarina/Lol_Katarina_Default.obj");

	// Set projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 1.0f, 100.0f);
	shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_inputManager.setDeltaTime(currentFrame - _inputManager.getLastFrame());
		_inputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Add transformation matrices
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(_inputManager._camera.GetViewMatrix()));
		glm::mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Add time component to geometry shader in the form of a uniform
		glUniform1f(glGetUniformLocation(shader.ID, "time"), currentFrame);

		// Draw model
		katarina.Draw(shader);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void render_instancing_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader planetShader = _resourceManager.loadShader("Shaders/advanced.vs", "Shaders/advanced.frag", "planetShader");
	genesis::Shader instanceShader = _resourceManager.loadShader("Shaders/instancing.vs", "Shaders/instancing.frag", "instanceShader");

	// Load models
	genesis::Model planet("Objects/Planet/planet.obj");
	genesis::Model rock("Objects/Planet/rock.obj");

	// Set projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 1.0f, 10000.0f);
	planetShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(planetShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// Also one for instance shader
	instanceShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(instanceShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

#pragma region "object_initialization"

	// Generate a large list of semi-random model transformation matrices
	GLuint amount = 1000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	GLfloat radius = 50.0f;
	GLfloat offset = 2.5f;
	for (GLuint i = 0; i < amount; i++)
	{
		glm::mat4 model;
		// 1. Translation: Randomly displace along circle with radius 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. Scale: Scale between 0.05 and 0.25f
		GLfloat scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
		GLfloat rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. Now add to list of matrices
		modelMatrices[i] = model;
	}

	// Set transformation matrices as an instance vertex attribute (with divisor 1)
	for (GLuint i = 0; i < rock.meshes.size(); i++)
	{
		GLuint VAO = rock.meshes[i].VAO;
		GLuint buffer;
		glBindVertexArray(VAO);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		// Set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

#pragma endregion

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_inputManager.setDeltaTime(currentFrame - _inputManager.getLastFrame());
		_inputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_inputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Add transformation matrices
		planetShader.Use();
		glm::mat4 view = _inputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(planetShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		instanceShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw Planet
		planetShader.Use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(glGetUniformLocation(planetShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		planet.Draw(planetShader);

		// Draw meteorites
		instanceShader.Use();
		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // Note we also made the textures_loaded vector public (instead of private) from the model class.
		for (GLuint i = 0; i < rock.meshes.size(); i++)
		{
			glBindVertexArray(rock.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].vertices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	delete[] modelMatrices;

	glfwTerminate();
}