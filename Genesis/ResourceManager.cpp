#include "ResourceManager.h"


namespace genesis {

	ResourceManager::ResourceManager()
	{
	}


	ResourceManager::~ResourceManager()
	{
	}

	GLuint ResourceManager::getCubemap()
	{
		return this->cubemapID;
	}

	// Loads a cubemap texture
	void ResourceManager::loadCubemap(vector<const GLchar*> faces)
	{
		glGenTextures(1, &this->cubemapID);

		int width, height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemapID);
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
		return this->textureID;
	}

	// Loads a texture from file.
	void ResourceManager::loadTexture(GLchar* path)
	{
		//Generate texture ID and load texture data 
		glGenTextures(1, &this->textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, this->textureID);
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

}