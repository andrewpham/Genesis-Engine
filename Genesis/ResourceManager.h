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

		GLuint loadCubemap(vector<const GLchar*>);
		GLuint loadTexture(GLchar*);
	};

}