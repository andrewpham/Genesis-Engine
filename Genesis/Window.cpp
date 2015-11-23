#include "Window.h"


namespace gwindow {


	Window::Window()
	{
	}


	Window::~Window()
	{
	}

	void Window::create(const char* windowName, GLuint screenWidth, GLuint screenHeight)
	{
		// Init GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		this->_window = glfwCreateWindow(screenWidth, screenHeight, windowName, nullptr, nullptr); // Windowed
		glfwMakeContextCurrent(_window);

		// Set the required callback functions
		glfwSetKeyCallback(_window, keyCallback);
		glfwSetCursorPosCallback(_window, mouseCallback);

		// Options
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	GLFWwindow* Window::getWindow()
	{
		return this->_window;
	}

}