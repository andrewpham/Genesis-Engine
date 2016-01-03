#pragma once
#include "GameObject3D.h"
#include <glm/gtc/quaternion.hpp>

namespace genesis {

	class Enemy :
		public GameObject3D
	{
	public:
		// Constructors
		Enemy(Shader _shader, Model _model, glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _size = glm::vec3(1.0), GLfloat _rotationAngleDegree = 0.0f,
			glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = true, GLboolean _destroyed = false, GLboolean _isAggroed = false, 
			glm::quat _orientation = glm::quat());
		Enemy(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _size = glm::vec3(1.0),
			GLfloat _rotationAngleDegree = 0.0f, glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = false, GLboolean _destroyed = false,
			GLboolean _isAggroed = false, glm::quat _orientation = glm::quat());
		// Getters and setters
		void setIsAggroed(GLboolean);
		GLboolean getIsAggroed();
		void setAggroRadius(GLfloat);
		GLfloat getAggroRadius();
		void setDamageRadius(GLfloat);
		GLfloat getDamageRadius();
		void setOrientation(glm::quat);
		glm::quat getOrientation();
	protected:
		// Enemy state
		GLboolean _isAggroed;
		GLfloat _aggroRadius;
		GLfloat _damageRadius;
		glm::quat _orientation;
	};

}