#pragma once
// Std. Includes
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h>
#include <SOIL.h>

namespace genesis {

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		GLuint getCubemap();
		void loadCubemap(vector<const GLchar*>);
		GLuint getTexture();
		void loadTexture(GLchar*);
	private:
		GLuint _cubemapID;
		GLuint _textureID;
	};

}