#pragma once
#include "GameObject3D.h"
#include <glm/gtc/quaternion.hpp>

namespace genesis {

	class Enemy :
		public GameObject3D
	{
	public:
		// Enemy state
		GLboolean _isAggroed;
		GLfloat _aggroRadius;
		GLfloat _damageRadius;
		glm::quat _orientation;
		// Constructor
		Enemy(Shader _shader, Model _model, glm::vec3 _translation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0), GLfloat _rotationAngleDegree = 0.0f, 
			glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = true, GLboolean _destroyed = false, GLboolean _isAggroed = false, glm::quat _orientation = glm::quat())
			: GameObject3D(_shader, _model, _translation, _scale, _rotationAngleDegree,_rotationAxis, _isModel, _destroyed), _isAggroed(_isAggroed), _orientation(_orientation) { }


		Enemy(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _translation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0),
			GLfloat _rotationAngleDegree = 0.0f, glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = false, GLboolean _destroyed = false, 
			GLboolean _isAggroed = false, glm::quat _orientation = glm::quat())
			: GameObject3D(_shader, _texture, _VAO, _numVertices, _translation, _scale, _rotationAngleDegree, _rotationAxis, _isModel, _destroyed), _isAggroed(_isAggroed), _orientation(_orientation) { }
	};

}