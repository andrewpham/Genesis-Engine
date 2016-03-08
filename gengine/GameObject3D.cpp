#include "GameObject3D.h"

namespace genesis {

	GameObject3D::GameObject3D()
	{
	}

	GameObject3D::GameObject3D(Shader _shader, Model _model, glm::vec3 _position, glm::vec3 _size,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _model(_model), _position(_position), _size(_size), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed), _hasNormalMap(false) { }

	GameObject3D::GameObject3D(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _position, glm::vec3 _size,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed)
		: _shader(_shader), _texture(_texture), _VAO(_VAO), _numVertices(_numVertices), _position(_position), _size(_size), 
		_rotationAngle(_rotationAngleDegree * PI_F / 180.0f), _rotationAxis(_rotationAxis), _isModel(_isModel), _destroyed(_destroyed), _hasNormalMap(false) { }

	GameObject3D::~GameObject3D()
	{
	}

	void GameObject3D::render()
	{
		if (!_destroyed)
		{
			_shader.Use();
			glUniform1i(glGetUniformLocation(_shader.ID, "hasNormalMap"), _hasNormalMap);
			glm::mat4 model;
			model = glm::translate(model, _position);
			model = glm::rotate(model, _rotationAngle, _rotationAxis);
			model = glm::rotate(model, _angularDisplacement.y, glm::vec3(0.0f, 0.0f, -1.0f));
			model = glm::rotate(model, _angularDisplacement.x, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, _angularDisplacement.z, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, _size);
			glUniformMatrix4fv(glGetUniformLocation(_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			if (_isModel)
				_model.Draw(_shader);
			else
			{
				glBindVertexArray(_VAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, _texture);
				if (_hasNormalMap)
				{
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, _normalMap);
				}
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

	void GameObject3D::setNormalMap(GLuint _normalMap)
	{
		this->_normalMap = _normalMap;
	}

	GLuint GameObject3D::getNormalMap()
	{
		return this->_normalMap;
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

	void GameObject3D::setVelocity(glm::vec3 _velocity)
	{
		this->_velocity = _velocity;
	}

	glm::vec3 GameObject3D::getVelocity()
	{
		return this->_velocity;
	}

	void GameObject3D::setAngularDisplacement(glm::vec3 _angularDisplacement)
	{
		this->_angularDisplacement = _angularDisplacement;
	}

	glm::vec3 GameObject3D::getAngularDisplacement()
	{
		return this->_angularDisplacement;
	}

	void GameObject3D::setAngularVelocity(glm::vec3 _angularVelocity)
	{
		this->_angularVelocity = _angularVelocity;
	}

	glm::vec3 GameObject3D::getAngularVelocity()
	{
		return this->_angularVelocity;
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

	void GameObject3D::setHasNormalMap(GLboolean _hasNormalMap)
	{
		this->_hasNormalMap = _hasNormalMap;
	}

	GLboolean GameObject3D::getHasNormalMap()
	{
		return this->_hasNormalMap;
	}

	void GameObject3D::setIsStatic(GLboolean _isStatic)
	{
		this->_isStatic = _isStatic;
	}

	GLboolean GameObject3D::getIsStatic()
	{
		return this->_isStatic;
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

	void GameObject3D::setNormals(std::vector<glm::vec3> _normals)
	{
		this->_normals = _normals;
	}

	std::vector<glm::vec3> GameObject3D::getNormals()
	{
		return this->_normals;
	}

}