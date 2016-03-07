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
		GLfloat _rotationAngle;
		// Constructors
		GameObject3D();
		GameObject3D(Shader _shader, Model _model, glm::vec3 _translation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0), 
			GLfloat _rotationAngleDegree = 0.0f, glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = true, GLboolean _destroyed = false);
		GameObject3D(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _translation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0), 
			GLfloat _rotationAngleDegree = 0.0f, glm::vec3 _rotationAxis = glm::vec3(0.0, 1.0, 0.0), GLboolean _isModel = false, GLboolean _destroyed = false);
		~GameObject3D();
		// Render object
		void render();
		// Getters and setters
		void setShader(Shader);
		Shader getShader();
		void setModel(Model);
		Model getModel();
		void setTexture(GLuint);
		GLuint getTexture();
		void setNormalMap(GLuint);
		GLuint getNormalMap();
		void setVAO(GLuint);
		GLuint getVAO();
		void setNumVertices(GLuint);
		GLuint getNumVertices();

		void setPosition(glm::vec3);
		glm::vec3 getPosition();
		void GameObject3D::setPositionY(GLfloat);
		void setVelocity(glm::vec3);
		glm::vec3 getVelocity();
		void setAngularDisplacement(glm::vec3);
		glm::vec3 getAngularDisplacement();
		void setAngularVelocity(glm::vec3);
		glm::vec3 getAngularVelocity();
		void setRotationAxis(glm::vec3);
		glm::vec3 getRotationAxis();
		void setSize(glm::vec3);
		glm::vec3 getSize();

		void setIsModel(GLboolean);
		GLboolean getIsModel();
		void setDestroyed(GLboolean);
		GLboolean getDestroyed();
		void setHasNormalMap(GLboolean);
		GLboolean getHasNormalMap();

		void setHitboxRadius(GLfloat);
		GLfloat getHitboxRadius();
		void setHitboxOffset(glm::vec3);
		glm::vec3 getHitboxOffset();

		void setNormals(std::vector<glm::vec3>);
		std::vector<glm::vec3> getNormals();
	protected:
		// Object state
		Shader _shader;
		Model _model;
		GLuint _texture;
		GLuint _normalMap;
		GLuint _VAO;
		GLuint _numVertices;

		glm::vec3 _position;
		glm::vec3 _velocity;
		glm::vec3 _angularDisplacement;
		glm::vec3 _angularVelocity;
		// For more granular and efficient rotations
		glm::vec3 _rotationAxis;
		glm::vec3 _size;

		GLboolean _isModel;
		GLboolean   _destroyed;
		GLboolean _hasNormalMap;

		GLfloat _hitboxRadius;
		glm::vec3 _hitboxOffset;

		std::vector<glm::vec3> _normals;
	};

}