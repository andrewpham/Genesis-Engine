#include "GameObject3D.h"

namespace genesis {

	GameObject3D::GameObject3D()
	{
	}

	GameObject3D::GameObject3D(Shader _shader, Model _model, glm::vec3 _translation, glm::vec3 _scale,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _model(_model), _translation(_translation), _scale(_scale), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed) { }

	GameObject3D::GameObject3D(Shader _shader, Texture2D _texture, glm::vec3 _translation, glm::vec3 _scale,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _texture(_texture), _translation(_translation), _scale(_scale), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed) { }

	GameObject3D::~GameObject3D()
	{
	}

	void GameObject3D::render()
	{
		_shader.Use();
		glm::mat4 model;
		model = glm::translate(model, _translation);
		model = glm::rotate(model, _rotationAngle, _rotationAxis);
		model = glm::scale(model, _scale);
		glUniformMatrix4fv(glGetUniformLocation(_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		_model.Draw(_shader);
	}

}