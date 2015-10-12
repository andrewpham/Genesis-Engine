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

// Multi-Draw Indirect Controls
MODE mode = MODE_MULTIDRAW;
bool paused = false;

// Additional Displacement Mapping Controls
float dmap_depth = false;
bool enable_displacement = false;
bool wireframe = false;
bool enable_fog = false;

// Additional Cubic Bezier Controls
bool show_points = false;
bool show_cage = false;

// Additional GS Quads Controls
int mode_no = 0;
int vid_offset = 0;

// Additional No Perspective Controls
bool use_perspective = true;

void render_superbible_depthclamp(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/depthclamp.vs", "Shaders/depthclamp.frag");

	GLint mv_location;
	GLint proj_location;
	GLint explode_factor_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.Program, "mv_matrix");
	proj_location = glGetUniformLocation(shader.Program, "proj_matrix");
	explode_factor_location = glGetUniformLocation(shader.Program, "explode_factor");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	glDeleteProgram(shader.Program);

	glfwTerminate();
}

void render_superbible_multiscissor(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/multiscissor.vs", "Shaders/multiscissor.frag", "Shaders/multiscissor.gs");

	GLuint          VAO;
	GLuint          position_buffer;
	GLuint          index_buffer;
	GLuint          uniform_buffer;
	GLint           mv_location;
	GLint           proj_location;

	shader.Use();

	mv_location = glGetUniformLocation(shader.Program, "mv_matrix");
	proj_location = glGetUniformLocation(shader.Program, "proj_matrix");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		// Turn on scissor testing
		glEnable(GL_SCISSOR_TEST);

		// Each rectangle will be 7/16 of the screen
		int scissor_width = (7 * screenWidth) / 16;
		int scissor_height = (7 * screenHeight) / 16;

		// Four rectangles - lower left first...
		glScissorIndexed(0,
			0, 0,
			scissor_width, scissor_height);

		// Lower right...
		glScissorIndexed(1,
			screenWidth - scissor_width, 0,
			scissor_width, scissor_height);

		// Upper left...
		glScissorIndexed(2,
			0, screenHeight - scissor_height,
			scissor_width, scissor_height);

		// Upper right...
		glScissorIndexed(3,
			screenWidth - scissor_width,
			screenHeight - scissor_height,
			scissor_width, scissor_height);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	glDeleteProgram(shader.Program);
	glDeleteBuffers(1, &position_buffer);

	glfwTerminate();
}

void render_superbible_noperspective(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/noperspective.vs", "Shaders/noperspective.frag");

	GLuint VAO;
	GLuint tex_checker;

	struct
	{
		GLint mvp;
		GLint use_perspective;
	} uniforms;

	shader.Use();

	uniforms.mvp = glGetUniformLocation(shader.Program, "mvp");
	uniforms.use_perspective = glGetUniformLocation(shader.Program, "use_perspective");

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
		Do_Movement();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!paused)
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time * 14.3f;

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -0.4f));
		mv_matrix = glm::rotate(mv_matrix, t * PI_F / 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 proj_matrix = glm::perspective(60.0f,
			(float)screenWidth / (float)screenHeight,
			0.1f, 1000.0f);

		shader.Use();

		glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
		glUniform1i(uniforms.use_perspective, use_perspective);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.Program);

	glfwTerminate();
}

void render_superbible_multiviewport(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/multiviewport.vs", "Shaders/multiviewport.frag", "Shaders/multiviewport.gs");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		// Each rectangle will be 7/16 of the screen
		float viewport_width = (float)(7 * screenWidth) / 16.0f;
		float viewport_height = (float)(7 * screenHeight) / 16.0f;

		// Four rectangles - lower left first...
		glViewportIndexedf(0, 0, 0, viewport_width, viewport_height);

		// Lower right...
		glViewportIndexedf(1,
			screenWidth - viewport_width, 0,
			viewport_width, viewport_height);

		// Upper left...
		glViewportIndexedf(2,
			0, screenHeight - viewport_height,
			viewport_width, viewport_height);

		// Upper right...
		glViewportIndexedf(3,
			screenWidth - viewport_width,
			screenHeight - viewport_height,
			viewport_width, viewport_height);

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	glDeleteProgram(shader.Program);
	glDeleteBuffers(1, &position_buffer);

	glfwTerminate();
}

void render_superbible_gsquads(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shaderFans("Shaders/quadsasfans.vs", "Shaders/quadsasfans.frag");
	Shader shaderLinesAdj("Shaders/quadsaslinesadj.vs", "Shaders/quadsaslinesadj.frag", "Shaders/quadsaslinesadj.gs");

	GLuint VAO;
	int mvp_loc_fans;
	int mvp_loc_linesadj;
	int vid_offset_loc_fans;
	int vid_offset_loc_linesadj;

	shaderFans.Use();

	mvp_loc_fans = glGetUniformLocation(shaderFans.Program, "mvp");
	vid_offset_loc_fans = glGetUniformLocation(shaderFans.Program, "vid_offset");

	shaderLinesAdj.Use();

	mvp_loc_linesadj = glGetUniformLocation(shaderLinesAdj.Program, "mvp");
	vid_offset_loc_linesadj = glGetUniformLocation(shaderLinesAdj.Program, "vid_offset");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear buffers
		static const GLfloat black[] = { 0.0f, 0.25f, 0.0f, 1.0f };
		glViewport(0, 0, screenWidth, screenHeight);

		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!paused)
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time;

		glClearBufferfv(GL_COLOR, 0, black);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -10.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)t * 5.0f * PI_F / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		mv_matrix = glm::rotate(mv_matrix, (float)t * 30.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 proj_matrix = glm::perspective(50.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
		glm::mat4 mvp = proj_matrix * mv_matrix;

		switch (mode_no)
		{
			case 0:
				shaderFans.Use();
				glUniformMatrix4fv(mvp_loc_fans, 1, GL_FALSE, glm::value_ptr(mvp));
				glUniform1i(vid_offset_loc_fans, vid_offset);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				break;
			case 1:
				shaderLinesAdj.Use();
				glUniformMatrix4fv(mvp_loc_linesadj, 1, GL_FALSE, glm::value_ptr(mvp));
				glUniform1i(vid_offset_loc_linesadj, vid_offset);
				glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
				break;
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shaderFans.Program);
	glDeleteProgram(shaderLinesAdj.Program);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
}

void render_superbible_normalviewer(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/normalviewer.vs", "Shaders/normalviewer.frag", "Shaders/normalviewer.gs");

	GLint mv_location;
	GLint proj_location;
	GLint normal_length_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.Program, "mv_matrix");
	proj_location = glGetUniformLocation(shader.Program, "proj_matrix");
	normal_length_location = glGetUniformLocation(shader.Program, "normal_length");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	Shader shader("Shaders/gstessellate.vs", "Shaders/gstessellate.frag", "Shaders/gstessellate.gs");

	GLint mv_location;
	GLint mvp_location;
	GLint stretch_location;
	GLuint vao;
	GLuint buffer;

	shader.Use();

	mv_location = glGetUniformLocation(shader.Program, "mvMatrix");
	mvp_location = glGetUniformLocation(shader.Program, "mvpMatrix");
	stretch_location = glGetUniformLocation(shader.Program, "stretch");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	Shader shader("Shaders/objectexploder.vs", "Shaders/objectexploder.frag", "Shaders/objectexploder.gs");

	GLint mv_location;
	GLint proj_location;
	GLint explode_factor_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.Program, "mv_matrix");
	proj_location = glGetUniformLocation(shader.Program, "proj_matrix");
	explode_factor_location = glGetUniformLocation(shader.Program, "explode_factor");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	Shader shader("Shaders/gsculling.vs", "Shaders/gsculling.frag", "Shaders/gsculling.gs");

	GLint mv_location;
	GLint mvp_location;
	GLint viewpoint_location;

	sb7::object object;

	shader.Use();

	mv_location = glGetUniformLocation(shader.Program, "mvMatrix");
	mvp_location = glGetUniformLocation(shader.Program, "mvpMatrix");
	viewpoint_location = glGetUniformLocation(shader.Program, "viewpoint");

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

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		glm::mat4 proj_matrix = glm::perspective(50.0f,
			(float)screenWidth / (float)screenHeight,
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
	glDeleteProgram(shader.Program);

	glfwTerminate();
}

void render_superbible_cubicbezier(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader tess_shader("Shaders/cubicbezier.vs", "Shaders/cubicbezier.frag", "Shaders/cubicbezier.tcs", "Shaders/cubicbezier.tes");
	Shader draw_cp_shader("Shaders/draw-control-points.vs", "Shaders/draw-control-points.frag");
	
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

	uniforms.patch.mv_matrix = glGetUniformLocation(tess_shader.Program, "mv_matrix");
	uniforms.patch.proj_matrix = glGetUniformLocation(tess_shader.Program, "proj_matrix");
	uniforms.patch.mvp = glGetUniformLocation(tess_shader.Program, "mvp");

	draw_cp_shader.Use();

	uniforms.control_point.draw_color = glGetUniformLocation(draw_cp_shader.Program, "draw_color");
	uniforms.control_point.mvp = glGetUniformLocation(draw_cp_shader.Program, "mvp");

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
		Do_Movement();

		// Clear buffers
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat one = 1.0f;

		int i;
		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!paused)
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

		glViewport(0, 0, screenWidth, screenHeight);
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
			(float)screenWidth / (float)screenHeight,
			1.0f, 1000.0f);

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(0.0f, 0.0f, -14.0f));
		mv_matrix = glm::rotate(mv_matrix, t * 10.0f * PI_F / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = glm::rotate(mv_matrix, t * 17.0f * PI_F / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniforms.patch.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.patch.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.patch.mvp, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));

		if (wireframe)
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

		if (show_points)
		{
			glPointSize(9.0f);
			glUniform4fv(uniforms.control_point.draw_color, 1, glm::value_ptr(glm::vec4(0.2f, 0.7f, 0.9f, 1.0f)));
			glDrawArrays(GL_POINTS, 0, 16);
		}

		if (show_cage)
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
	Shader shader("Shaders/dispmap.vs", "Shaders/dispmap.frag", "Shaders/dispmap.tcs", "Shaders/dispmap.tes");

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

	uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
	uniforms.mvp_matrix = glGetUniformLocation(shader.Program, "mvp_matrix");
	uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
	uniforms.dmap_depth = glGetUniformLocation(shader.Program, "dmap_depth");
	uniforms.enable_fog = glGetUniformLocation(shader.Program, "enable_fog");
	dmap_depth = 6.0f;

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
		Do_Movement();

		// Clear buffers
		static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0;
		static double total_time = 0.0;

		GLfloat currentTime = glfwGetTime();
		if (!paused)
			total_time += (currentTime - last_time);
		last_time = currentTime;

		float t = (float)total_time * 0.03f;
		float r = sinf(t * 5.37f) * 15.0f + 16.0f;
		float h = cosf(t * 4.79f) * 2.0f + 3.2f;

		glViewport(0, 0, screenWidth, screenHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glm::mat4 mv_matrix = glm::lookAt(glm::vec3(sinf(t) * r, h, cosf(t) * r), glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		glm::mat4 proj_matrix = glm::perspective(60.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);

		shader.Use();

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(uniforms.mvp_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
		glUniform1f(uniforms.dmap_depth, enable_displacement ? dmap_depth : 0.0f);
		glUniform1i(uniforms.enable_fog, enable_fog ? 1 : 0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_PATCHES, GL_FILL);
		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shader.Program);

	glfwTerminate();
}

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

void render_superbible_multidrawindirect(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/multidrawindirect.vs", "Shaders/multidrawindirect.frag");

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

void render_superbible_instancedattribs(GLFWwindow* window)
{
	// Setup and compile our shaders
	Shader shader("Shaders/instancedattribs.vs", "Shaders/instancedattribs.frag");

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
	// SB controls
	if (keys[GLFW_KEY_P])
		paused = !paused;
	if (keys[GLFW_KEY_D])
	{
		mode = MODE(mode + 1);
		if (mode > MODE_MAX)
			mode = MODE_FIRST;
	}

	if (keys[GLFW_KEY_APOSTROPHE])
		dmap_depth += 0.1f;
	if (keys[GLFW_KEY_SEMICOLON])
		dmap_depth -= 0.1f;
	if (keys[GLFW_KEY_F])
		enable_fog = !enable_fog;
	if (keys[GLFW_KEY_G])
		enable_displacement = !enable_displacement;
	if (keys[GLFW_KEY_H])
		wireframe = !wireframe;

	if (keys[GLFW_KEY_C])
		show_cage = !show_cage;
	if (keys[GLFW_KEY_X])
		show_points = !show_points;

	if (keys[GLFW_KEY_1])
		mode_no = 0;
	if (keys[GLFW_KEY_2])
		mode_no = 1;
	if (keys[GLFW_KEY_SLASH])
		vid_offset++;
	if (keys[GLFW_KEY_PERIOD])
		vid_offset--;
	if (keys[GLFW_KEY_M])
		mode_no = (mode_no + 1) % 2;
	if (keys[GLFW_KEY_U])
		use_perspective = !use_perspective;
}

#pragma endregion