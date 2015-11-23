#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

namespace genesis {

	class Shader
	{
	public:
		GLuint Program;
		// Constructor generates the shader on the fly
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
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
			const GLchar * fShaderCode = fragmentCode.c_str();
			// 2. Compile shaders
			GLuint vertex, fragment;
			GLint success;
			GLchar infoLog[512];
			// Vertex Shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			// Print compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			// Print compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Shader Program
			this->Program = glCreateProgram();
			glAttachShader(this->Program, vertex);
			glAttachShader(this->Program, fragment);
			glLinkProgram(this->Program);
			// Print linking errors if any
			glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			// Delete the shaders as they're linked into our program now and no longer necessery
			glDeleteShader(vertex);
			glDeleteShader(fragment);

		}
		// Constructor generates the shader on the fly
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* tessellationControlPath, const GLchar* tessellationEvaluationPath)
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
			const GLchar * fShaderCode = fragmentCode.c_str();
			const GLchar* tcShaderCode = tessellationControlCode.c_str();
			const GLchar* teShaderCode = tessellationEvaluationCode.c_str();
			// 2. Compile shaders
			GLuint vertex, fragment, tessellationControl, tessellationEvaluation;
			GLint success;
			GLchar infoLog[512];
			// Vertex Shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			// Print compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			// Print compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Tessellation Control Shader
			tessellationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tessellationControl, 1, &tcShaderCode, NULL);
			glCompileShader(tessellationControl);
			// Print compile errors if any
			glGetShaderiv(tessellationControl, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(tessellationControl, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::TESSELLATION_CONTROL::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Tessellation Evaluation Shader
			tessellationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(tessellationEvaluation, 1, &teShaderCode, NULL);
			glCompileShader(tessellationEvaluation);
			// Print compile errors if any
			glGetShaderiv(tessellationEvaluation, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(tessellationEvaluation, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::TESSELLATION_EVALUATION::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Shader Program
			this->Program = glCreateProgram();
			glAttachShader(this->Program, vertex);
			glAttachShader(this->Program, fragment);
			glAttachShader(this->Program, tessellationControl);
			glAttachShader(this->Program, tessellationEvaluation);
			glLinkProgram(this->Program);
			// Print linking errors if any
			glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			// Delete the shaders as they're linked into our program now and no longer necessery
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			glDeleteShader(tessellationControl);
			glDeleteShader(tessellationEvaluation);

		}
		// Constructor generates the shader on the fly
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
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
			const GLchar * fShaderCode = fragmentCode.c_str();
			const GLchar* gShaderCode = geometryCode.c_str();
			// 2. Compile shaders
			GLuint vertex, fragment, geometry;
			GLint success;
			GLchar infoLog[512];
			// Vertex Shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			// Print compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			// Print compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Geometry Shader
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			//  Print compile errors if any
			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(geometry, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Shader Program
			this->Program = glCreateProgram();
			glAttachShader(this->Program, vertex);
			glAttachShader(this->Program, fragment);
			glAttachShader(this->Program, geometry);
			glLinkProgram(this->Program);
			// Print linking errors if any
			glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			// Delete the shaders as they're linked into our program now and no longer necessery
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			glDeleteShader(geometry);

		}
		// Uses the current shader
		void Use()
		{
			glUseProgram(this->Program);
		}
	};

}

#endif