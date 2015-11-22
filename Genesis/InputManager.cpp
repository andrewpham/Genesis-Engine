#include "InputManager.h"



InputManager::InputManager() : _camera(glm::vec3(0.0f, 0.0f, 3.0f)), _lastX(400), _lastY(300), _firstMouse(true), _deltaTime(0.0f), _lastFrame(0.0f), 
							   _mode(MODE_MULTIDRAW), _paused(false), _dmapDepth(false), _enableDisplacement(false), _wireframe(false), _enableFog(false),
							   _showPoints(false), _showCage(false), _modeNo(0), _vidOffset(0), _usePerspective(true), _exposure(1.0f)
{

}


InputManager::~InputManager()
{
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void InputManager::checkKeysPressed()
{
	// Camera controls
	if (_keys[GLFW_KEY_W])
	{
		_camera.ProcessKeyboard(FORWARD, _deltaTime);

		_wireframe = !_wireframe;
	}
	if (_keys[GLFW_KEY_S])
		_camera.ProcessKeyboard(BACKWARD, _deltaTime);
	if (_keys[GLFW_KEY_A])
		_camera.ProcessKeyboard(LEFT, _deltaTime);
	if (_keys[GLFW_KEY_D])
		_camera.ProcessKeyboard(RIGHT, _deltaTime);
	// SB controls
	if (_keys[GLFW_KEY_P])
		_paused = !_paused;
	if (_keys[GLFW_KEY_D])
	{
		_mode = MODE(_mode + 1);
		if (_mode > MODE_MAX)
			_mode = MODE_FIRST;

		_enableDisplacement = !_enableDisplacement;
	}

	if (_keys[GLFW_KEY_F])
		_enableFog = !_enableFog;

	if (_keys[GLFW_KEY_C])
		_showCage = !_showCage;
	if (_keys[GLFW_KEY_X])
		_showPoints = !_showPoints;

	if (_keys[GLFW_KEY_1])
		_modeNo = 0;
	if (_keys[GLFW_KEY_2])
		_modeNo = 1;
	if (_keys[GLFW_KEY_M]) // Altered for HDR Tone Mapping!
	{
		_modeNo = (_modeNo + 1) % 3;

		_usePerspective = !_usePerspective;
	}

	// New Keybinds for HDR Tone Mapping
	if (_keys[GLFW_KEY_3])
		_modeNo = 2;
	if (_keys[GLFW_KEY_EQUAL])
	{
		_dmapDepth += 0.1f;

		_vidOffset++;

		_exposure *= 1.1f;
	}
	if (_keys[GLFW_KEY_MINUS])
	{
		_dmapDepth -= 0.1f;

		_vidOffset--;

		_exposure /= 1.1f;
	}
}

#pragma endregion

GLfloat InputManager::getLastX()
{
	return this->_lastX;
}

void InputManager::setLastX(GLfloat _lastX)
{
	this->_lastX = _lastX;
}

GLfloat InputManager::getLastY()
{
	return this->_lastY;
}

void InputManager::setLastY(GLfloat _lastY)
{
	this->_lastY = _lastY;
}

bool InputManager::getFirstMouse()
{
	return this->_firstMouse;
}

void InputManager::setFirstMouse(bool _firstMouse)
{
	this->_firstMouse = _firstMouse;
}

void InputManager::setDeltaTime(GLfloat _deltaTime)
{
	this->_deltaTime = _deltaTime;
}

GLfloat InputManager::getLastFrame()
{
	return this->_lastFrame;
}

void InputManager::setLastFrame(GLfloat _lastFrame)
{
	this->_lastFrame = _lastFrame;
}

MODE InputManager::getMode()
{
	return this->_mode;
}

bool InputManager::getPaused()
{
	return this->_paused;
}

float InputManager::getDmapDepth()
{
	return this->_dmapDepth;
}

void InputManager::setDmapDepth(float _dmapDepth)
{
	this->_dmapDepth = _dmapDepth;
}

bool InputManager::getEnableDisplacement()
{
	return this->_enableDisplacement;
}

bool InputManager::getWireframe()
{
	return this->_wireframe;
}

bool InputManager::getEnableFog()
{
	return this->_enableFog;
}

bool InputManager::getShowPoints()
{
	return this->_showPoints;
}

bool InputManager::getShowCage()
{
	return this->_showCage;
}

int InputManager::getModeNo()
{
	return this->_modeNo;
}

int InputManager::getVidOffset()
{
	return this->_vidOffset;
}

bool InputManager::getUsePerspective()
{
	return this->_usePerspective;
}

float InputManager::getExposure()
{
	return this->_exposure;
}