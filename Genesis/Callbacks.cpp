#include "Callbacks.h"

namespace genesis {
	// Is called whenever a key is pressed/released via GLFW
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		if (action == GLFW_PRESS)
			_inputManager._keys[key] = true;
		else if (action == GLFW_RELEASE)
			_inputManager._keys[key] = false;
	}

	void mouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (_inputManager.getFirstMouse())
		{
			_inputManager.setLastX(xpos);
			_inputManager.setLastY(ypos);
			_inputManager.setFirstMouse(false);
		}

		GLfloat xoffset = xpos - _inputManager.getLastX();
		GLfloat yoffset = _inputManager.getLastY() - ypos;

		_inputManager.setLastX(xpos);
		_inputManager.setLastY(ypos);

		_inputManager._camera.ProcessMouseMovement(xoffset, yoffset);
	}
}