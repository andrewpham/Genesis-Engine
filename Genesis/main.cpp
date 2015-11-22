// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GL includes
#include "Demos.h"

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		inputManager._keys[key] = true;
	else if (action == GLFW_RELEASE)
		inputManager._keys[key] = false;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (inputManager.getFirstMouse())
	{
		inputManager.setLastX(xpos);
		inputManager.setLastY(ypos);
		inputManager.setFirstMouse(false);
	}

	GLfloat xoffset = xpos - inputManager.getLastX();
	GLfloat yoffset = inputManager.getLastY() - ypos;

	inputManager.setLastX(xpos);
	inputManager.setLastY(ypos);

	inputManager._camera.ProcessMouseMovement(xoffset, yoffset);
}

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
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

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
	//render_superbible_shapedpoints(window);
	//render_superbible_starfield(window);
	render_superbible_hdrtonemap(window);
	//render_superbible_polygonsmooth(window);
	//render_superbible_linesmooth(window);
	//render_superbible_basicfbo(window);
	//render_superbible_depthclamp(window);
	//render_superbible_multiscissor(window);
	//render_superbible_noperspective(window);
	//render_superbible_multiviewport(window);
	//render_superbible_gsquads(window);
	//render_superbible_normalviewer(window);
	//render_superbible_gstessellate(window);
	//render_superbible_objectexploder(window);
	//render_superbible_gsculling(window);
	//render_superbible_cubicbezier(window);
	//render_superbible_dispmap(window);
	//render_superbible_tessmodes(window);
	//render_superbible_clipdistance(window);
	//render_superbible_multidrawindirect(window);
	//render_superbible_instancedattribs(window);
	//render_superbible_fragmentlist(window);
	//render_skybox_demo(window);
	//render_exploding_demo(window);
	//render_instancing_demo(window);

	return 0;
}