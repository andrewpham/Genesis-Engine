/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "GameLevel.h"


namespace genesis {

	void GameLevel::load(const GLchar *_file, GLuint _levelWidth, GLuint _levelHeight)
	{
		// Clear old data
		this->_bricks.clear();
		// Load from file
		GLuint tileCode;
		GameLevel level;
		std::string line;
		std::ifstream fstream(_file);
		std::vector<std::vector<GLuint>> tileData;
		if (fstream)
		{
			while (std::getline(fstream, line)) // Read each line from level file
			{
				std::istringstream sstream(line);
				std::vector<GLuint> row;
				while (sstream >> tileCode) // Read each word seperated by spaces
					row.push_back(tileCode);
				tileData.push_back(row);
			}
			if (tileData.size() > 0)
				this->init(tileData, _levelWidth, _levelHeight);
		}
	}

	void GameLevel::draw(SpriteRenderer &_renderer)
	{
		for (GameObject &tile : this->_bricks)
			if (!tile._destroyed)
				tile.draw(_renderer);
	}

	GLboolean GameLevel::isCompleted()
	{
		for (GameObject &tile : this->_bricks)
			if (!tile._isSolid && !tile._destroyed)
				return GL_FALSE;
		return GL_TRUE;
	}

	void GameLevel::init(std::vector<std::vector<GLuint>> _tileData, GLuint _levelWidth, GLuint _levelHeight)
	{
		// Calculate dimensions
		GLuint height = _tileData.size();
		GLuint width = _tileData[0].size(); // Note we can index vector at [0] since this function is only called if height > 0
		GLfloat unit_width = _levelWidth / static_cast<GLfloat>(width), unit_height = _levelHeight / height;
		// Initialize level tiles based on tileData		
		for (GLuint y = 0; y < height; ++y)
		{
			for (GLuint x = 0; x < width; ++x)
			{
				// Check block type from level data (2D level array)
				if (_tileData[y][x] == 1) // Solid
				{
					glm::vec2 pos(unit_width * x, unit_height * y);
					glm::vec2 size(unit_width, unit_height);
					GameObject obj(pos, size, ResourceManager::getTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
					obj._isSolid = GL_TRUE;
					this->_bricks.push_back(obj);
				}
				else if (_tileData[y][x] > 1)	// Non-solid; now determine its color based on level data
				{
					glm::vec3 color = glm::vec3(1.0f); // original: white
					if (_tileData[y][x] == 2)
						color = glm::vec3(0.2f, 0.6f, 1.0f);
					else if (_tileData[y][x] == 3)
						color = glm::vec3(0.0f, 0.7f, 0.0f);
					else if (_tileData[y][x] == 4)
						color = glm::vec3(0.8f, 0.8f, 0.4f);
					else if (_tileData[y][x] == 5)
						color = glm::vec3(1.0f, 0.5f, 0.0f);

					glm::vec2 pos(unit_width * x, unit_height * y);
					glm::vec2 size(unit_width, unit_height);
					this->_bricks.push_back(GameObject(pos, size, ResourceManager::getTexture("block"), color));
				}
			}
		}
	}

}