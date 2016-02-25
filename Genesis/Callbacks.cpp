#include "Callbacks.h"

namespace gwindow {
	// Is called whenever a key is pressed/released via GLFW
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		// Additional keybinds for the Breakout game object
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				Breakout._keys[key] = true;
			else if (action == GLFW_RELEASE)
				Breakout._keys[key] = false;
		}

		// Additional keybinds for the rest of the game engine demos
		if (action == GLFW_PRESS)
		{
			_inputManager._keys[key] = true; _gabenGameInputManager._keys[key] = true; _skinDemoInputManager._keys[key] = true; _physicsSimInputManager._keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			_inputManager._keys[key] = false; _gabenGameInputManager._keys[key] = false; _skinDemoInputManager._keys[key] = false; _physicsSimInputManager._keys[key] = false;
		}
	}

	void mouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (_inputManager.getFirstMouse())
		{
			_inputManager.setLastX(xpos); _gabenGameInputManager.setLastX(xpos); _skinDemoInputManager.setLastX(xpos); _physicsSimInputManager.setLastX(xpos);
			_inputManager.setLastY(ypos); _gabenGameInputManager.setLastY(ypos); _skinDemoInputManager.setLastY(ypos); _physicsSimInputManager.setLastY(ypos);
			_inputManager.setFirstMouse(false); _gabenGameInputManager.setFirstMouse(false); _skinDemoInputManager.setFirstMouse(false); _physicsSimInputManager.setFirstMouse(false);
		}

		GLfloat xoffset = xpos - _inputManager.getLastX();
		GLfloat yoffset = _inputManager.getLastY() - ypos;

		_inputManager.setLastX(xpos); _gabenGameInputManager.setLastX(xpos); _skinDemoInputManager.setLastX(xpos); _physicsSimInputManager.setLastX(xpos);
		_inputManager.setLastY(ypos); _gabenGameInputManager.setLastY(ypos); _skinDemoInputManager.setLastY(ypos); _physicsSimInputManager.setLastY(ypos);

		_inputManager._camera.ProcessMouseMovement(xoffset, yoffset); _gabenGameInputManager._camera.ProcessMouseMovement(xoffset, yoffset); 
		_skinDemoInputManager._camera.ProcessMouseMovement(xoffset, yoffset); _physicsSimInputManager._camera.ProcessMouseMovement(xoffset, yoffset);
	}
}