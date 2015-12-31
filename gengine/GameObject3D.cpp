#include "GameObject3D.h"

namespace genesis {

	GameObject3D::GameObject3D()
	{
	}

	GameObject3D::GameObject3D(Shader _shader, Model _model, glm::vec3 _translation, glm::vec3 _scale,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _model(_model), _translation(_translation), _scale(_scale), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed) { }

	GameObject3D::GameObject3D(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _translation, glm::vec3 _scale,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _texture(_texture), _VAO(_VAO), _numVertices(_numVertices), _translation(_translation), _scale(_scale), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed) { }

	GameObject3D::~GameObject3D()
	{
	}

	void GameObject3D::render()
	{
		if (!_destroyed)
		{
			_shader.Use();
			glm::mat4 model;
			model = glm::translate(model, _translation);
			model = glm::rotate(model, _rotationAngle, _rotationAxis);
			model = glm::scale(model, _scale);
			glUniformMatrix4fv(glGetUniformLocation(_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			if (_isModel)
				_model.Draw(_shader);
			else
			{
				glBindVertexArray(_VAO);
				glBindTexture(GL_TEXTURE_2D, _texture);
				glDrawArrays(GL_TRIANGLES, 0, _numVertices);
				glBindVertexArray(0);
			}
		}
	}

}