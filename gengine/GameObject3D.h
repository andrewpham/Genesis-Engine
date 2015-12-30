#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "Texture2D.h"
#include "Shader.h"
#include "Model.h"
#include "Variables.h"

namespace genesis {

	// Container object for holding all state relevant for a single
	// game object entity. Each object in the game likely needs the
	// minimal of state as described within GameObject.
	class GameObject3D
	{
	public:
		// Object state
		Shader _shader;
		Model _model;
		Texture2D _texture;

		glm::vec3 _translation;
		GLfloat _rotationAngle;
		glm::vec3 _rotationAxis;
		glm::vec3 _scale;

		GLboolean _isModel;
		GLboolean   _destroyed;

		GLfloat _hitboxRadius;
		glm::vec3 _hitboxOffset;
		// Constructors
		GameObject3D();
		GameObject3D(Shader _shader, Model _model, glm::vec3 _translation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0), 
			GLfloat _rotationAngleDegree = 0.0f, glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = true, GLboolean _destroyed = false);
		GameObject3D(Shader _shader, Texture2D _texture, glm::vec3 _translation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0), 
			GLfloat _rotationAngleDegree = 0.0f, glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = false, GLboolean _destroyed = false);
		~GameObject3D();
		// Render object
		virtual void render();
	};

}