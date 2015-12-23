/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "Shader.h"

namespace genesis {

	Shader &Shader::Use()
	{
		glUseProgram(this->ID);
		return *this;
	}

	void Shader::compile(const GLchar* vertexSource, const GLchar* fragmentSource)
	{
		// Compile shaders
		GLuint sVertex, sFragment;
		// Vertex Shader
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vertexSource, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");
		// Fragment Shader
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fragmentSource, NULL);
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

	void Shader::compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource)
	{
		// Compile shaders
		GLuint sVertex, sFragment, gShader;
		// Vertex Shader
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vertexSource, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");
		// Fragment Shader
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fragmentSource, NULL);
		glCompileShader(sFragment);
		checkCompileErrors(sFragment, "FRAGMENT");
		// Geometry shader
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
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

	void Shader::compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* tessellationControlSource, const GLchar* tessellationEvaluationSource)
	{
		// Compile shaders
		GLuint sVertex, sFragment, tcShader, teShader;
		// Vertex Shader
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vertexSource, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");
		// Fragment Shader
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fragmentSource, NULL);
		glCompileShader(sFragment);
		checkCompileErrors(sFragment, "FRAGMENT");
		// Tessellation Control Shader
		tcShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tcShader, 1, &tessellationControlSource, NULL);
		glCompileShader(tcShader);
		checkCompileErrors(tcShader, "TESSELLATION CONTROL");
		// Tessellation Evaluation Shader
		teShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(teShader, 1, &tessellationEvaluationSource, NULL);
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

	void Shader::compile(const GLchar* computeSource)
	{
		// Compile shaders
		GLuint cShader;
		// Compute Shader
		cShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(cShader, 1, &computeSource, NULL);
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

	void Shader::setFloat(const GLchar *name, GLfloat value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform1f(glGetUniformLocation(this->ID, name), value);
	}
	void Shader::setInteger(const GLchar *name, GLint value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform1i(glGetUniformLocation(this->ID, name), value);
	}
	void Shader::setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), x, y);
	}
	void Shader::setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
	}
	void Shader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
	}
	void Shader::setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
	}
	void Shader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
	}
	void Shader::setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
	}
	void Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
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