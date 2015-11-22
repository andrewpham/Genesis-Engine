#pragma once
#include "Camera.h"
using namespace std;

// GLM Mathematics
#include <glm/glm.hpp>

// GLFW
#include <GLFW/glfw3.h>

enum MODE
{
	MODE_FIRST,
	MODE_MULTIDRAW = 0,
	MODE_SEPARATE_DRAWS,
	MODE_MAX = MODE_SEPARATE_DRAWS
};

namespace genesis {

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void checkKeysPressed();

		Camera _camera;
		bool _keys[1024];

		GLfloat getLastX();
		void setLastX(GLfloat);
		GLfloat getLastY();
		void setLastY(GLfloat);
		bool getFirstMouse();
		void setFirstMouse(bool);

		void setDeltaTime(GLfloat);
		GLfloat getLastFrame();
		void setLastFrame(GLfloat);

		MODE getMode();
		bool getPaused();

		float getDmapDepth();
		void setDmapDepth(float);
		bool getEnableDisplacement();
		bool getWireframe();
		bool getEnableFog();

		bool getShowPoints();
		bool getShowCage();

		int getModeNo();
		int getVidOffset();

		bool getUsePerspective();

		float getExposure();
	private:
		// Camera
		GLfloat _lastX, _lastY;
		bool _firstMouse;

		GLfloat _deltaTime;
		GLfloat _lastFrame;

		// Multi-Draw Indirect Controls
		MODE _mode;
		bool _paused;

		// Additional Displacement Mapping Controls
		float _dmapDepth;
		bool _enableDisplacement;
		bool _wireframe;
		bool _enableFog;

		// Additional Cubic Bezier Controls
		bool _showPoints;
		bool _showCage;

		// Additional GS Quads Controls
		int _modeNo;
		int _vidOffset;

		// Additional No Perspective Controls
		bool _usePerspective;

		// Additional HDR Tone Mapping Controls
		float _exposure;
	};

}