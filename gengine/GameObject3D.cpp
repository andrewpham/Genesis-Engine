#include "GameObject3D.h"

namespace genesis {

	GameObject3D::GameObject3D()
	{
	}

	GameObject3D::GameObject3D(Shader _shader, Model _model, glm::vec3 _position, glm::vec3 _size,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _model(_model), _position(_position), _size(_size), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed) { }

	GameObject3D::GameObject3D(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _position, glm::vec3 _size,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _texture(_texture), _VAO(_VAO), _numVertices(_numVertices), _position(_position), _size(_size), 
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
			model = glm::translate(model, _position);
			model = glm::rotate(model, _rotationAngle, _rotationAxis);
			model = glm::scale(model, _size);
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

	void GameObject3D::setShader(Shader _shader)
	{
		this->_shader = _shader;
	}

	Shader GameObject3D::getShader()
	{
		return this->_shader;
	}

	void GameObject3D::setModel(Model _model)
	{
		this->_model = _model;
	}

	Model GameObject3D::getModel()
	{
		return this->_model;
	}

	void GameObject3D::setTexture(GLuint _texture)
	{
		this->_texture = _texture;
	}

	GLuint GameObject3D::getTexture()
	{
		return this->_texture;
	}

	void GameObject3D::setVAO(GLuint _VAO)
	{
		this->_VAO = _VAO;
	}

	GLuint GameObject3D::getVAO()
	{
		return this->_VAO;
	}

	void GameObject3D::setNumVertices(GLuint _numVertices)
	{
		this->_numVertices = _numVertices;
	}

	GLuint GameObject3D::getNumVertices()
	{
		return this->_numVertices;
	}

	void GameObject3D::setPosition(glm::vec3 _position)
	{
		this->_position = _position;
	}

	glm::vec3 GameObject3D::getPosition()
	{
		return this->_position;
	}

	void GameObject3D::setPositionY(GLfloat _y)
	{
		this->_position.y = _y;
	}

	void GameObject3D::setRotationAxis(glm::vec3 _rotationAxis)
	{
		this->_rotationAxis = _rotationAxis;
	}

	glm::vec3 GameObject3D::getRotationAxis()
	{
		return this->_rotationAxis;
	}

	void GameObject3D::setSize(glm::vec3 _size)
	{
		this->_size = _size;
	}

	glm::vec3 GameObject3D::getSize()
	{
		return this->_size;
	}

	void GameObject3D::setIsModel(GLboolean _isModel)
	{
		this->_isModel = _isModel;
	}

	GLboolean GameObject3D::getIsModel()
	{
		return this->_isModel;
	}

	void GameObject3D::setDestroyed(GLboolean _destroyed)
	{
		this->_destroyed = _destroyed;
	}

	GLboolean GameObject3D::getDestroyed()
	{
		return this->_destroyed;
	}

	void GameObject3D::setHitboxRadius(GLfloat _hitboxRadius)
	{
		this->_hitboxRadius = _hitboxRadius;
	}

	GLfloat GameObject3D::getHitboxRadius()
	{
		return this->_hitboxRadius;
	}

	void GameObject3D::setHitboxOffset(glm::vec3 _hitboxOffset)
	{
		this->_hitboxOffset = _hitboxOffset;
	}

	glm::vec3 GameObject3D::getHitboxOffset()
	{
		return this->_hitboxOffset;
	}

}