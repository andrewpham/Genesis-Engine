/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace genesis {

	Shader &Shader::Use()
	{
		glUseProgram(this->ID);
		return *this;
	}

	void Shader::Compile(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		// 2. Compile shaders
		GLuint sVertex, sFragment;
		// Vertex Shader
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vShaderCode, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");
		// Fragment Shader
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fShaderCode, NULL);
		glCompileShader(sFragment);
		checkCompileErrors(sFragment, "FRAGMENT");
		// Shader Program
		this->ID = glCreateProgram();
		glAttachShader(this->ID, sVertex);
		glAttachShader(this->ID, sFragment);
		glLinkProgram(this->ID);
		checkCompileErrors(this->ID, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
	}

	void Shader::Compile(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			gShaderFile.open(geometryPath);
			std::stringstream vShaderStream, fShaderStream, gShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			gShaderStream << gShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			gShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		const GLchar* gShaderCode = geometryCode.c_str();
		// 2. Compile shaders
		GLuint sVertex, sFragment, gShader;
		// Vertex Shader
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vShaderCode, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");
		// Fragment Shader
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fShaderCode, NULL);
		glCompileShader(sFragment);
		checkCompileErrors(sFragment, "FRAGMENT");
		// If geometry shader source code is given, also compile geometry shader
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &gShaderCode, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
		// Shader Program
		this->ID = glCreateProgram();
		glAttachShader(this->ID, sVertex);
		glAttachShader(this->ID, sFragment);
		glAttachShader(this->ID, gShader);
		glLinkProgram(this->ID);
		checkCompileErrors(this->ID, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		glDeleteShader(gShader);
	}

	void Shader::Compile(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* tessellationControlPath, const GLchar* tessellationEvaluationPath)
	{
		// 1. Retrieve the source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string tessellationControlCode;
		std::string tessellationEvaluationCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream tcShaderFile;
		std::ifstream teShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		tcShaderFile.exceptions(std::ifstream::badbit);
		teShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			tcShaderFile.open(tessellationControlPath);
			teShaderFile.open(tessellationEvaluationPath);
			std::stringstream vShaderStream, fShaderStream, tcShaderStream, teShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			tcShaderStream << tcShaderFile.rdbuf();
			teShaderStream << teShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			tcShaderFile.close();
			teShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			tessellationControlCode = tcShaderStream.str();
			tessellationEvaluationCode = teShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		const GLchar* tcShaderCode = tessellationControlCode.c_str();
		const GLchar* teShaderCode = tessellationEvaluationCode.c_str();
		// 2. Compile shaders
		GLuint sVertex, sFragment, tcShader, teShader;
		// Vertex Shader
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vShaderCode, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");
		// Fragment Shader
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fShaderCode, NULL);
		glCompileShader(sFragment);
		checkCompileErrors(sFragment, "FRAGMENT");
		// Tessellation Control Shader
		tcShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tcShader, 1, &tcShaderCode, NULL);
		glCompileShader(tcShader);
		checkCompileErrors(tcShader, "TESSELLATION CONTROL");
		// Tessellation Evaluation Shader
		teShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(teShader, 1, &teShaderCode, NULL);
		glCompileShader(teShader);
		checkCompileErrors(teShader, "TESSELLATION EVALUATION");
		// Shader Program
		this->ID = glCreateProgram();
		glAttachShader(this->ID, sVertex);
		glAttachShader(this->ID, sFragment);
		glAttachShader(this->ID, tcShader);
		glAttachShader(this->ID, teShader);
		glLinkProgram(this->ID);
		checkCompileErrors(this->ID, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		glDeleteShader(tcShader);
		glDeleteShader(teShader);
	}

	void Shader::Compile(const GLchar* computePath)
	{
		// 1. Retrieve the source code from filePath
		std::string computeCode;
		std::ifstream cShaderFile;
		// ensures ifstream objects can throw exceptions:
		cShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			cShaderFile.open(computePath);
			std::stringstream cShaderStream;
			// Read file's buffer contents into streams
			cShaderStream << cShaderFile.rdbuf();
			// close file handlers
			cShaderFile.close();
			// Convert stream into string
			computeCode = cShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* cShaderCode = computeCode.c_str();
		// 2. Compile shaders
		GLuint cShader;
		// Vertex Shader
		cShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(cShader, 1, &cShaderCode, NULL);
		glCompileShader(cShader);
		checkCompileErrors(cShader, "COMPUTE");
		// Shader Program
		this->ID = glCreateProgram();
		glAttachShader(this->ID, cShader);
		glLinkProgram(this->ID);
		checkCompileErrors(this->ID, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(cShader);
	}

	void Shader::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform1f(glGetUniformLocation(this->ID, name), value);
	}
	void Shader::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform1i(glGetUniformLocation(this->ID, name), value);
	}
	void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), x, y);
	}
	void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
	}
	void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
	}
	void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
	}
	void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
	}
	void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
	}
	void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}


	void Shader::checkCompileErrors(GLuint object, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(object, 1024, NULL, infoLog);
				std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
					<< infoLog << "\n -- --------------------------------------------------- -- "
					<< std::endl;
			}
		}
		else
		{
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(object, 1024, NULL, infoLog);
				std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
					<< infoLog << "\n -- --------------------------------------------------- -- "
					<< std::endl;
			}
		}
	}

}