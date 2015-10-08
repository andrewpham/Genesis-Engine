// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GL includes
#include "Demos.h"

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Genesis", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options - disable when rendering SB code!
	//glEnable(GL_DEPTH_TEST);

	// Render demo
	render_superbible_dispmap(window);
	//render_superbible_tessmodes(window);
	//render_superbible_clipdistance(window);
	//render_superbible_asteroids(window);
	//render_superbible_squares(window);
	//render_superbible_fragmentlist(window);
	//render_superbible_demo(window);
	//render_skybox_demo(window);
	//render_exploding_demo(window);
	//render_instancing_demo(window);

	return 0;
}