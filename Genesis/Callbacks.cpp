#include "Callbacks.h"

namespace genesis {
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
}