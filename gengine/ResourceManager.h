#pragma once
// Std. Includes
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;
// GL Includes
#include <GL/glew.h>
#include <SOIL.h>

#include "Texture2D.h"
#include "Shader.h"

namespace genesis {

	// A static singleton ResourceManager class that hosts several
	// functions to load Textures and Shaders. Each loaded texture
	// and/or shader is also stored for future reference by string
	// handles. All functions and resources are static and no 
	// public constructor is defined.
	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		GLuint getCubemap();
		void loadCubemap(vector<const GLchar*>);
		GLuint getTexture();
		void loadTexture(GLchar*);

		// Resource storage
		static std::map<std::string, Shader> _shaders;
		static std::map<std::string, Texture2D> _textures;
		// Loads (and generates) a shader program from file loading vertex, fragment and perhaps shader source code.
		static Shader loadShader(const GLchar *vShaderPath, const GLchar *fShaderPath, std::string name);
		static Shader loadShader(const GLchar *vShaderPath, const GLchar *fShaderPath, const GLchar *gShaderPath, std::string name);
		static Shader loadShader(const GLchar *vShaderPath, const GLchar *fShaderPath, const GLchar *tcShaderPath, const GLchar *teShaderPath, std::string name);
		static Shader loadShader(const GLchar *cShaderPath, std::string name);
		// Retrieves a stored shader
		static Shader getShader(std::string name);
		// Loads (and generates) a texture from file
		static Texture2D loadTexture(const GLchar *file, GLboolean alpha, std::string name);
		// Retrieves a stored texture
		static Texture2D getTexture(std::string name);
	private:
		GLuint _cubemapID;
		GLuint _textureID;
		// Loads and generates a shader from file
		static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile);
		static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile);
		static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *tcShaderFile, const GLchar *teShaderFile);
		static Shader loadShaderFromFile(const GLchar *cShaderFile);
		// Loads a single texture from file
		static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
	};

}