/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace genesis {

	// General purpsoe shader object. Compiles from file, generates
	// compile/link-time error messages and hosts several utility 
	// functions for easy management.
	class Shader
	{
	public:
		// State
		GLuint ID;
		// Constructor
		Shader() { }
		// Sets the current shader as active
		Shader  &Use();
		// Compiles the shader from given source code
		void    compile(const GLchar *vertexPath, const GLchar *fragmentPath);
		void    compile(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath); 
		void	compile(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *tessellationControlPath, const GLchar *tessellationEvaluationPath);
		void	compile(const GLchar *computePath);
																														   // Utility functions
		void    setFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
		void    setInteger(const GLchar *name, GLint value, GLboolean useShader = false);
		void    setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
		void    setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
		void    setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
		void    setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
		void    setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
		void    setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
		void    setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);
	private:
		// Checks if compilation or linking failed and if so, print the error logs
		void    checkCompileErrors(GLuint object, std::string type);
	};

}

#endif