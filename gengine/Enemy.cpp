#include "Enemy.h"

namespace genesis {

	Enemy::Enemy(Shader _shader, Model _model, glm::vec3 _position, glm::vec3 _size, GLfloat _rotationAngleDegree,
		glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed, GLboolean _isAggroed, glm::quat _orientation)
		: GameObject3D(_shader, _model, _position, _size, _rotationAngleDegree, _rotationAxis, _isModel, _destroyed), 
		_isAggroed(_isAggroed), _orientation(_orientation), _health(5) { }

	Enemy::Enemy(Shader _shader, GLuint _texture, GLuint _VAO, GLuint _numVertices, glm::vec3 _position, glm::vec3 _size,
		GLfloat _rotationAngleDegree, glm::vec3 _rotationAxis, GLboolean _isModel, GLboolean _destroyed, GLboolean _isAggroed, 
		glm::quat _orientation)
		: GameObject3D(_shader, _texture, _VAO, _numVertices, _position, _size, _rotationAngleDegree, _rotationAxis, _isModel, _destroyed), 
		_isAggroed(_isAggroed), _orientation(_orientation), _health(5) { }

	void Enemy::setIsAggroed(GLboolean _isAggroed)
	{
		this->_isAggroed = _isAggroed;
	}

	GLboolean Enemy::getIsAggroed()
	{
		return this->_isAggroed;
	}

	void Enemy::setAggroRadius(GLfloat _aggroRadius)
	{
		this->_aggroRadius = _aggroRadius;
	}

	GLfloat Enemy::getAggroRadius()
	{
		return this->_aggroRadius;
	}

	void Enemy::setDamageRadius(GLfloat _damageRadius)
	{
		this->_damageRadius = _damageRadius;
	}

	GLfloat Enemy::getDamageRadius()
	{
		return this->_damageRadius;
	}

	void Enemy::setOrientation(glm::quat _orientation)
	{
		this->_orientation = _orientation;
	}

	glm::quat Enemy::getOrientation()
	{
		return this->_orientation;
	}

	void Enemy::setHealth(GLuint _health)
	{
		this->_health = _health;
	}

	GLuint Enemy::getHealth()
	{
		return this->_health;
	}

}