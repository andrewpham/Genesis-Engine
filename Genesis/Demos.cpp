// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Demos.h"
#include "sb7ktx.h"
#include "sb7object.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

const float PI_F = 3.14159265358979f;
GLuint tex_index = 0;
MODE mode = MODE_MULTIDRAW;
bool paused = false;
bool vsync = false;

void render_superbible_tessmodes(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader0("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_quads.tcs", "Shaders/tessmodes_quads.tes");
	Shader shader1("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_triangles.tcs", "Shaders/tessmodes_triangles.tes");
	Shader shader2("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_triangles.tcs", "Shaders/tessmodes_triangles_as_points.tes");
	Shader shader3("Shaders/tessmodes.vs", "Shaders/tessmodes.frag", "Shaders/tessmodes_isolines.tcs", "Shaders/tessmodes_isolines.tes");

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
	Shader shader("Shaders/clipdistance.vs", "Shaders/clipdistance.frag");

	sb7::object object;

	struct
	{
		GLint proj_matrix;
		GLint mv_matrix;
		GLint clip_plane;
		GLint clip_sphere;
	} uniforms;

	shader.Use();
	uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
	uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
	uniforms.clip_plane = glGetUniformLocation(shader.Program, "clip_plane");
	uniforms.clip_sphere = glGetUniformLocation(shader.Program, "clip_sphere");

	object.load("sb7objects/dragon.sbm");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat one = 1.0f;

		static double last_time = 0.0;
		static double total_time = 0.0;

		// Set frame time
		GLfloat currentTime = glfwGetTime();
		if (!paused)
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float f = (float)total_time;

		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -80.0f));
		mv_matrix = glm::rotate(mv_matrix, f * 0.34f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, -4.0f, 0.0f));

		glm::mat4 plane_matrix;
		plane_matrix = glm::rotate(plane_matrix, f * 6.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		plane_matrix = glm::rotate(plane_matrix, f * 7.3f, glm::vec3(0.0f, 1.0f, 0.0f));

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

void render_superbible_asteroids(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/asteroids.vs", "Shaders/asteroids.frag");

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

	uniforms.time = glGetUniformLocation(shader.Program, "time");
	uniforms.view_matrix = glGetUniformLocation(shader.Program, "view_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
	uniforms.viewproj_matrix = glGetUniformLocation(shader.Program, "viewproj_matrix");

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
		Do_Movement();

		int j;
		static const float one = 1.0f;
		static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };


		static double last_time = 0.0;
		static double total_time = 0.0;

		// Set frame time
		GLfloat currentTime = glfwGetTime();
		if (!paused)
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = float(total_time);
		int i = int(total_time * 3.0f);

		// Clear buffers
		glViewport(0, 0, (float)screenWidth, (float)screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		const glm::mat4 view_matrix = glm::lookAt(glm::vec3(100.0f * cosf(t * 0.023f), 100.0f * cosf(t * 0.023f), 300.0f * sinf(t * 0.037f) - 600.0f),
			glm::vec3(0.0f, 0.0f, 260.0f),
			glm::normalize(glm::vec3(0.1f - cosf(t * 0.1f) * 0.3f, 1.0f, 0.0f)));
		const glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
			1.0f,
			2000.0f);

		shader.Use();

		glUniform1f(uniforms.time, t);
		glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.viewproj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix * view_matrix));

		glBindVertexArray(object.get_vao());

		if (mode == MODE_MULTIDRAW)
		{
			glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, NUM_DRAWS, 0);
		}
		else if (mode == MODE_SEPARATE_DRAWS)
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

void render_superbible_squares(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/square.vs", "Shaders/square.frag");

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
	Shader clearShader("Shaders/clear.vs", "Shaders/clear.frag");
	Shader appendShader("Shaders/append.vs", "Shaders/append.frag");
	Shader resolveShader("Shaders/resolve.vs", "Shaders/resolve.frag");

	appendShader.Use();
	uniforms.mvp = glGetUniformLocation(appendShader.Program, "mvp");

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

		// Clear buffers
		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };

		glViewport(0, 0, (float)screenWidth, (float)screenHeight);

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
		glm::mat4 proj_matrix = glm::perspective(50.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);

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

void render_superbible_demo(GLFWwindow* window)
{
#define B 0x00, 0x00, 0x00, 0x00
#define W 0xFF, 0xFF, 0xFF, 0xFF
	static const GLubyte tex_data[] =
	{
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
		W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	};
#undef B
#undef W

	// Setup and compile our shaders
	Shader shader("Shaders/superbible.vs", "Shaders/superbible.frag");

	GLuint tex_object[2];

	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
	} uniforms;

	sb7::object object;

	shader.Use();
	uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");

	glGenTextures(1, &tex_object[0]);
	glBindTexture(GL_TEXTURE_2D, tex_object[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 16, 16);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	tex_object[1] = sb7::ktx::file::load("Textures/pattern1.ktx");

	object.load("sb7objects/torus_nrms_tc.sbm");

	glDepthFunc(GL_LEQUAL);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear buffers
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glViewport(0, 0, (GLfloat)screenWidth, (GLfloat)screenHeight);

		glBindTexture(GL_TEXTURE_2D, tex_object[tex_index]);

		// Draw cubes
		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(60.0f, ((float)screenWidth) / ((float)screenHeight), 0.1f, 1000.0f);
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -1.8f));
		mv_matrix = glm::rotate(mv_matrix, (GLfloat)currentFrame * 19.3f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, (GLfloat)currentFrame * 21.1f * PI_F / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		object.render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader.Program);
	glDeleteTextures(2, tex_object);

	glfwTerminate();
}

void render_skybox_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/advanced.vs", "Shaders/advanced.frag");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.frag");

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
	GLuint cubeTexture = loadTexture("Textures/container.jpg");
#pragma endregion

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("Skybox/right.jpg");
	faces.push_back("Skybox/left.jpg");
	faces.push_back("Skybox/top.jpg");
	faces.push_back("Skybox/bottom.jpg");
	faces.push_back("Skybox/back.jpg");
	faces.push_back("Skybox/front.jpg");
	GLuint cubemapTexture = loadCubemap(faces);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw skybox first
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Draw cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		// Then draw scene as normal
		shader.Use();
		glm::mat4 model;
		view = camera.GetViewMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.Program, "texture_diffuse1"), 0);
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
	Shader shader("Shaders/geometry.vs", "Shaders/geometry.frag", "Shaders/geometry.gs");

	// Load models
	Model katarina("Katarina/Lol_Katarina_Default.obj");

	// Set projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 100.0f);
	shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Add transformation matrices
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
		glm::mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Add time component to geometry shader in the form of a uniform
		glUniform1f(glGetUniformLocation(shader.Program, "time"), currentFrame);

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
	Shader planetShader("Shaders/advanced.vs", "Shaders/advanced.frag");
	Shader instanceShader("Shaders/instancing.vs", "Shaders/instancing.frag");

	// Load models
	Model planet("objects/planet.obj");
	Model rock("objects/rock.obj");

	// Set projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 10000.0f);
	planetShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(planetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// Also one for instance shader
	instanceShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(instanceShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

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

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear buffers
		glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Add transformation matrices
		planetShader.Use();
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(planetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		instanceShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw Planet
		planetShader.Use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(glGetUniformLocation(planetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
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

// Loads a cubemap texture
GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

// Loads a texture from file.
GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

#pragma region "User input"

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_T])
	{
		tex_index++;
		if (tex_index > 1)
			tex_index = 0;
	}
	if (keys[GLFW_KEY_P])
		paused = !paused;
	if (keys[GLFW_KEY_D])
	{
		mode = MODE(mode + 1);
		if (mode > MODE_MAX)
			mode = MODE_FIRST;
	}
}

#pragma endregion

void generate_texture(float * data, int width, int height)
{
	int x, y;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 3] = 1.0f;
		}
	}
}