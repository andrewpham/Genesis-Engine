// GL includes
#include <gengine/Model.h>
#include "RigidBodySim.h"

genesis::InputManager _physicsSimInputManager;
genesis::ResourceManager _physicsSimResourceManager;
std::vector<glm::vec3> _normals { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f),
									glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f) };

void getInterval(genesis::GameObject3D&, glm::vec3, float&, float&);
bool intersectsOnNormal(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3, const glm::vec3&, const glm::vec3&, float&);
bool findMTD(std::vector<glm::vec3>, std::vector<float>, glm::vec3&, float&);
bool checkCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);

void run_physics_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _physicsSimResourceManager.loadShader("Shaders/Rigid Body Sim/object.vs", "Shaders/Rigid Body Sim/object.frag", "shader");

	struct
	{
		GLint view;
		GLint projection;
	} uniforms;

	// Cache the uniform locations
	shader.Use();
	uniforms.view = glGetUniformLocation(shader.ID, "view");
	uniforms.projection = glGetUniformLocation(shader.ID, "projection");

	// Used to pace the spawning of objects at regular intervals
	GLfloat attackCooldown = 0.0f;

	// Load models
	genesis::Model floor("Objects/Crate/Crate2.obj");
	genesis::Model box("Objects/Crate/Crate1.obj");

	// Create game objects
	genesis::GameObject3D floorObject(shader, floor, glm::vec3(0.0f, -2.0f, 0.0f));
	floorObject.setHitboxRadius(1.0f);
	floorObject.setNormals(_normals);
	vector<genesis::GameObject3D> boxObjects;
	// Top left spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 0.5f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.0f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.5f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.0f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.5f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.0f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.5f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.0f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.5f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.0f, -0.875f)));
	// Top right spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, -0.75f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, -0.45f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, -0.15f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.15f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.45f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.75f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.05f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.35f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.65f, -0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.95f, -0.875f)));
	// Bottom left spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 0.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.0f, 0.875f)));
	// Bottom right spawn
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 2.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 2.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 3.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 3.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 4.0f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 4.5f, 0.875f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 5.0f, 0.875f)));

	// Set the hitbox radii
	for (genesis::GameObject3D &boxObject : boxObjects)
	{
		boxObject.setHitboxRadius(0.125f);
		boxObject.setNormals(_normals);
	}

	// Set projection matrix
	glm::mat4 projection = glm::perspective(_physicsSimInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_physicsSimInputManager.setDeltaTime(currentFrame - _physicsSimInputManager.getLastFrame());
		_physicsSimInputManager.setLastFrame(currentFrame);
		// Controls the gunshot cooldown and later resets the cooldown timer when we fire a box
		if (attackCooldown > 0.0f)
			attackCooldown -= _physicsSimInputManager.getDeltaTime();

		// Check and call events
		glfwPollEvents();
		_physicsSimInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Spawns a box every time we fire
		if (_physicsSimInputManager._keys[GLFW_KEY_SPACE] && attackCooldown <= 0.0f)
		{
			_physicsSimInputManager.getSoundEngine()->play2D("../Genesis/Audio/Rigid Body Sim/boxspawn.wav", GL_FALSE);
			boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(_physicsSimInputManager._camera.Position.x + _physicsSimInputManager._camera.Front.x, _physicsSimInputManager._camera.Position.y + _physicsSimInputManager._camera.Front.y, _physicsSimInputManager._camera.Position.z + _physicsSimInputManager._camera.Front.z)));
			boxObjects.back().setVelocity(5.0f * _physicsSimInputManager._camera.Front);
			attackCooldown = 0.5f;
		}

		// Set view matrix
		glm::mat4 view = _physicsSimInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(view));

		// Draw the floor
		floorObject.render();
		// Draw the boxes
		for (genesis::GameObject3D &boxObject : boxObjects)
		{
			boxObject.render();
			boxObject.setVelocity(boxObject.getVelocity() + G_CONST * _physicsSimInputManager.getDeltaTime());
			boxObject.setPosition(boxObject.getPosition() + boxObject.getVelocity() * _physicsSimInputManager.getDeltaTime());
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void getInterval(genesis::GameObject3D &_object, glm::vec3 _normal, float &_minRange, float &_maxRange)
{
	_minRange = std::numeric_limits<float>::max();
	_maxRange = std::numeric_limits<float>::min();
	float r = _object.getHitboxRadius();
	vector<glm::vec3> vertices;
	glm::mat4 rotationMat;
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, -1.0f));
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 rotation = glm::mat3(rotationMat);
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, -r, -r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, -r, -r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, r, -r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, -r, r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, r, -r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, -r, r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, r, r));
	vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, r, r));

	for (glm::vec3 vertex : vertices)
	{
		float pointOnNormal = glm::dot(_normal, vertex);
		if (pointOnNormal < _minRange)
			_minRange = pointOnNormal;
		if (pointOnNormal > _maxRange)
			_maxRange = pointOnNormal;
	}
}

bool intersectsOnNormal(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 _normal, const glm::vec3 &_offset, const glm::vec3 &_xVel, float &_t)
{
	float min0, max0;	// Projected interval onto the normal of the first object
	float min1, max1;	// Projected interval onto the normal of the second object

	// Project the vertices of our objects onto the normal and extract the intervals on the normal
	getInterval(_object1, _normal, min0, max0);
	getInterval(_object2, _normal, min1, max1);

	// h helps translates our objects into local space
	float h = glm::dot(_offset, _normal);
	min0 += h;
	max0 += h;

	float d0 = min0 - max1; // If overlap, d0 < 0
	float d1 = min1 - max0; // If overlap, d1 < 0
	// If separated, test dynamic intervals
	float v;
	float t0, t1, temp;
	if (d0 > 0.0f || d1 > 0.0f) {
		v = glm::dot(_xVel, _normal);
		
		// Small velocity, so only the overlap test will be relevant
		if (fabs(v) < 0.0000001f) return false;

		t0 = -d0 / v;	// Time of impact until d0 reaches 0
		t1 = d1 / v;	// Time of impact until d1 reaches 1

		if (t0 > t1)
		{
			temp = t0;
			t0 = t1;
			t1 = temp;
		}

		_t = (t0 > 0.0f) ? t0 : t1;

		if (_t < 0.0f || _t > 1.0f)
			return false;

		return true;
	}
	else {
		// If there is overlap, get the overlap interval
		_t = (d0 > d1) ? d0 : d1;
		return true;
	}

	return false;
}

/** Determines the maximum penetration depth given a list of penetration depths */
bool findMTD(std::vector<glm::vec3> _axis, std::vector<float> _penetration, glm::vec3 &_normal, float &_t)
{
	bool penetrationExists = false;
	_t = std::numeric_limits<float>::max();
	int i;

	for (i = 0; i < _axis.size(); i++)
	{
		if (_penetration[i] < 0.0f)
		{
			penetrationExists = true;
			if (_penetration[i] < _t)
			{
				_t = _penetration[i];
				_normal = _axis[i];
			}
		}
	}

	return penetrationExists;
}

/** Checks the two rigid bodies to see if a collision occurs, and calculates the contact normal and collision type descriptor (t) */
bool checkCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_contactN, float &_t)
{
	glm::vec3 offset = _object2.getPosition() - _object1.getPosition();
	float xLength = glm::length(offset);

	glm::vec3 xVel = _object2.getVelocity() - _object1.getVelocity();
	std::vector<float> depth(12);
	std::vector<glm::vec3> axis(12);

	int i, j = 0;
	float t = 1.0f;

	/** Check if there exists an axis that separates the two objects */
	for (i = 0; i < 6; i++)
	{
		// Returns false if there exists any axis or normal that separates the two bounds
		if (!intersectsOnNormal(_object1, _object2, _object1.getNormals()[i], offset, xVel, t))
			return false;
		axis[j] = _object1.getNormals()[i];
		depth[j] = t;
		j++;
	}
	for (i = 0; i < 6; i++)
	{
		if (!intersectsOnNormal(_object2, _object1, _object2.getNormals()[i], offset, xVel, t))
			return false;
		axis[j] = _object2.getNormals()[i];
		depth[j] = t;
		j++;
	}

	// Contact normal
	glm::vec3 N;

	if (!findMTD(axis, depth, N, t))
		return false;

	if (glm::dot(N, offset) < 0.0f)
		N = glm::vec3(-N.x, -N.y, -N.z);
	N = glm::vec3(-N.x, -N.y, -N.z);
	if (N.x == -0)
		N.x = 0;
	if (N.y == -0)
		N.y = 0;
	if (N.z == -0)
		N.z = 0;

	_contactN = N;
	_t = t;

	return true;
}