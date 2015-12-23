#include "ResourceManager.h"

namespace genesis {

	// Instantiate static variables
	std::map<std::string, Texture2D> ResourceManager::Textures;
	std::map<std::string, Shader> ResourceManager::Shaders;

	ResourceManager::ResourceManager()
	{
	}


	ResourceManager::~ResourceManager()
	{
	}

	GLuint ResourceManager::getCubemap()
	{
		return this->_cubemapID;
	}

	// Loads a cubemap texture
	void ResourceManager::loadCubemap(vector<const GLchar*> faces)
	{
		glGenTextures(1, &this->_cubemapID);

		int width, height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, this->_cubemapID);
		for (GLuint i = 0; i < faces.size(); i++)
		{
			image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	GLuint ResourceManager::getTexture()
	{
		return this->_textureID;
	}

	// Loads a texture from file.
	void ResourceManager::loadTexture(GLchar* path)
	{
		//Generate texture ID and load texture data 
		glGenTextures(1, &this->_textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, this->_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);
	}

	Shader ResourceManager::loadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, std::string name)
	{
		Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile);
		return Shaders[name];
	}

	Shader ResourceManager::loadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name)
	{
		Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
		return Shaders[name];
	}

	Shader ResourceManager::loadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *tcShaderFile, const GLchar *teShaderFile, std::string name)
	{
		Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, tcShaderFile, teShaderFile);
		return Shaders[name];
	}

	Shader ResourceManager::loadShader(const GLchar *cShaderFile, std::string name)
	{
		Shaders[name] = loadShaderFromFile(cShaderFile);
		return Shaders[name];
	}

	Shader ResourceManager::getShader(std::string name)
	{
		return Shaders[name];
	}

	Texture2D ResourceManager::loadTexture(const GLchar *file, GLboolean alpha, std::string name)
	{
		Textures[name] = loadTextureFromFile(file, alpha);
		return Textures[name];
	}

	Texture2D ResourceManager::getTexture(std::string name)
	{
		return Textures[name];
	}

	Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile)
	{
		// 1. Retrieve the vertex/fragment source code from file path
		std::string vertexCode;
		std::string fragmentCode;
		try
		{
			// Open files
			std::ifstream vertexShaderFile(vShaderFile);
			std::ifstream fragmentShaderFile(fShaderFile);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vertexShaderFile.rdbuf();
			fShaderStream << fragmentShaderFile.rdbuf();
			// Close file handlers
			vertexShaderFile.close();
			fragmentShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::exception e)
		{
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const GLchar *vShaderCode = vertexCode.c_str();
		const GLchar *fShaderCode = fragmentCode.c_str();
		// 2. Now create shader object from source code
		Shader shader;
		shader.compile(vShaderCode, fShaderCode);
		return shader;
	}

	Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
	{
		// 1. Retrieve the source code from file paths
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		try
		{
			// Open files
			std::ifstream vertexShaderFile(vShaderFile);
			std::ifstream fragmentShaderFile(fShaderFile);
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream vShaderStream, fShaderStream, gShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vertexShaderFile.rdbuf();
			fShaderStream << fragmentShaderFile.rdbuf();
			gShaderStream << geometryShaderFile.rdbuf();
			// Close file handlers
			vertexShaderFile.close();
			fragmentShaderFile.close();
			geometryShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			geometryCode = gShaderStream.str();
		}
		catch (std::exception e)
		{
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const GLchar *vShaderCode = vertexCode.c_str();
		const GLchar *fShaderCode = fragmentCode.c_str();
		const GLchar *gShaderCode = geometryCode.c_str();
		// 2. Now create shader object from source code
		Shader shader;
		shader.compile(vShaderCode, fShaderCode, gShaderCode);
		return shader;
	}

	Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *tcShaderFile, const GLchar *teShaderFile)
	{
		// 1. Retrieve the source code from file paths
		std::string vertexCode;
		std::string fragmentCode;
		std::string tessellationControlCode;
		std::string tessellationEvaluationCode;
		try
		{
			// Open files
			std::ifstream vertexShaderFile(vShaderFile);
			std::ifstream fragmentShaderFile(fShaderFile);
			std::ifstream tessellationControlShaderFile(tcShaderFile);
			std::ifstream tessellationEvaluationShaderFile(teShaderFile);
			std::stringstream vShaderStream, fShaderStream, tcShaderStream, teShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vertexShaderFile.rdbuf();
			fShaderStream << fragmentShaderFile.rdbuf();
			tcShaderStream << tessellationControlShaderFile.rdbuf();
			teShaderStream << tessellationEvaluationShaderFile.rdbuf();
			// Close file handlers
			vertexShaderFile.close();
			fragmentShaderFile.close();
			tessellationControlShaderFile.close();
			tessellationEvaluationShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			tessellationControlCode = tcShaderStream.str();
			tessellationEvaluationCode = teShaderStream.str();
		}
		catch (std::exception e)
		{
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const GLchar *vShaderCode = vertexCode.c_str();
		const GLchar *fShaderCode = fragmentCode.c_str();
		const GLchar *tcShaderCode = tessellationControlCode.c_str();
		const GLchar *teShaderCode = tessellationEvaluationCode.c_str();
		// 2. Now create shader object from source code
		Shader shader;
		shader.compile(vShaderCode, fShaderCode, tcShaderCode, teShaderCode);
		return shader;
	}

	Shader ResourceManager::loadShaderFromFile(const GLchar *cShaderFile)
	{
		// 1. Retrieve compute source code from file path
		std::string computeCode;
		try
		{
			// Open file
			std::ifstream computeShaderFile(cShaderFile);
			std::stringstream cShaderStream;
			// Read file's buffer contents into streams
			cShaderStream << computeShaderFile.rdbuf();
			// Close file handler
			computeShaderFile.close();
			// Convert stream into string
			computeCode = cShaderStream.str();
		}
		catch (std::exception e)
		{
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const GLchar *cShaderCode = computeCode.c_str();
		// 2. Now create shader object from source code
		Shader shader;
		shader.compile(cShaderCode);
		return shader;
	}

	Texture2D ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
	{
		// Create Texture object
		Texture2D texture;
		if (alpha)
		{
			texture._internalFormat = GL_RGBA;
			texture._imageFormat = GL_RGBA;
		}
		// Load image
		int width, height;
		unsigned char* image = SOIL_load_image(file, &width, &height, 0, texture._imageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
		// Now generate texture
		texture.generate(width, height, image);
		// And finally free image data
		SOIL_free_image_data(image);
		return texture;
	}

}