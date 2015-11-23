#pragma once
#include "Callbacks.h"

namespace genesis {

	class Window
	{
	public:
		Window();
		~Window();

		void create(const char*, GLuint, GLuint);
		GLFWwindow* getWindow();
	private:
		GLFWwindow* _window;
	};

}