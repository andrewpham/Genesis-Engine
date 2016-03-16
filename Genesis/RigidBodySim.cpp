// GL includes
#include <gengine/Model.h>
#include "RigidBodySim.h"

genesis::InputManager _physicsSimInputManager;
genesis::ResourceManager _physicsSimResourceManager;
std::vector<glm::vec3> _normals { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f),
									glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f) };

void getInterval(genesis::GameObject3D&, glm::vec3, float&, float&);
bool intersectsOnNormal(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3, const glm::vec3&, const glm::vec3&, float&);
bool findMTD(std::vector<glm::vec3>&, std::vector<float>&, glm::vec3&, float&);
bool checkCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);
bool checkAABBCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);
void resolveCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3, float, float);

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
	vector<genesis::GameObject3D> boxObjects;
	// Floor
	boxObjects.push_back(genesis::GameObject3D(shader, floor, glm::vec3(0.0f, -1.5f, 0.0f)));
	// Boxes
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 1.5f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 2.0f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 2.5f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 3.0f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 3.5f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 4.0f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 4.5f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 5.0f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 5.5f, 0.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.0f, 6.0f, 0.0f)));
	//// Top left spawn
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 0.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.0f, -0.875f)));
	//// Top right spawn
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, -0.75f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, -0.45f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, -0.15f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.15f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.45f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.75f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.05f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.35f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.65f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.95f, -0.875f)));
	//// Bottom left spawn
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 0.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.0f, 0.875f)));
	//// Bottom right spawn
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 0.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 2.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 2.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 3.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 3.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 4.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 4.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 5.0f, 0.875f)));

	// Set the hitbox radii and normals
	for (genesis::GameObject3D &boxObject : boxObjects)
	{
		boxObject.setHitboxRadius(0.125f);
		boxObject.setNormals(_normals);
		boxObject.setIsStatic(false);
	}
	boxObjects[0].setHitboxRadius(1.0f);
	boxObjects[0].setIsStatic(true);

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
			boxObjects.back().setHitboxRadius(0.125f);
			boxObjects.back().setNormals(_normals);
			boxObjects.back().setIsStatic(false);
			attackCooldown = 0.5f;
		}

		// Set view matrix
		glm::mat4 view = _physicsSimInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(view));

		// Contact normal
		glm::vec3 N;
		// Collision type descriptor (t < 0 = penetration occurred)
		float t = 0.0f;
		bool collisionExists = false;

		int i, j;
		for (i = 0; i < boxObjects.size(); i++)
		{
			N.x = 0;
			N.y = 0;
			N.z = 0;
			t = 0.0f;

			for (j = 0; j < boxObjects.size(); j++)
			{
				if (i == j)
					continue;
				if (i != 0 && j != 0 && fabs(glm::length(boxObjects[i].getPosition() - boxObjects[j].getPosition())) > 0.6)
					continue;
				if (i == 0 || j == 0 && fabs(glm::length(boxObjects[i].getPosition() - boxObjects[j].getPosition())) > 2)
					continue;

				//collisionExists = checkCollision(boxObjects[i], boxObjects[j], N, t);
				collisionExists = checkAABBCollision(boxObjects[i], boxObjects[j], N, t);
				if (collisionExists && t < 0)
				{
					resolveCollision(boxObjects[i], boxObjects[j], -N, t, _physicsSimInputManager.getDeltaTime());
				}
				//else if (!boxObjects[i].getIsStatic())
				//{
				//	boxObjects[i].setVelocity(boxObjects[i].getVelocity() + G_CONST * _physicsSimInputManager.getDeltaTime());
				//}
				//boxObjects[i].setPosition(boxObjects[i].getPosition() + boxObjects[i].getVelocity() * _physicsSimInputManager.getDeltaTime());
			}
		}

		// Draw the boxes
		for (genesis::GameObject3D &boxObject : boxObjects)
		{
			boxObject.render();
			//  Update step
			if (!boxObject.getIsStatic())
			{
				if (boxObject.getPosition().y - boxObject.getHitboxRadius() > -0.5 || 
					boxObject.getPosition().x > 1.125 || boxObject.getPosition().x < -1.125 || 
					boxObject.getPosition().z > 1.125 || boxObject.getPosition().z < -1.125)
				{
					boxObject.setVelocity(boxObject.getVelocity() + G_CONST * _physicsSimInputManager.getDeltaTime());
					boxObject.setPosition(boxObject.getPosition() + boxObject.getVelocity() * _physicsSimInputManager.getDeltaTime());
					boxObject.setAngularDisplacement(boxObject.getAngularDisplacement() + boxObject.getAngularVelocity() * _physicsSimInputManager.getDeltaTime());
				}
				else
				{
					boxObject.setVelocity(glm::vec3(boxObject.getVelocity().x, 0.0f, boxObject.getVelocity().z));
				}
			}
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
bool findMTD(std::vector<glm::vec3> &_axis, std::vector<float> &_penetration, glm::vec3 &_normal, float &_t)
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

bool checkAABBCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_contactN, float &_t)
{
	float r0 = _object1.getHitboxRadius();
	glm::vec3 min0 = _object1.getPosition() - glm::vec3(r0, r0, r0);
	glm::vec3 max0 = _object1.getPosition() + glm::vec3(r0, r0, r0);
	float r1 = _object2.getHitboxRadius();
	glm::vec3 min1 = _object2.getPosition() - glm::vec3(r1, r1, r1);
	glm::vec3 max1 = _object2.getPosition() + glm::vec3(r1, r1, r1);

	bool penetrationExists = false;
	penetrationExists = (min0.x <= max1.x && max0.x >= min1.x) &&
		(min0.y <= max1.y && max0.y >= min1.y) &&
		(min0.z <= max1.z && max0.z >= min1.z);

	float penX = 0, penY = 0, penZ = 0;
	if (penetrationExists && min0.x <= max1.x && max0.x >= min1.x)
		penX = min(max1.x - min0.x, max0.x - min1.x);
	if (penetrationExists && min0.y <= max1.y && max0.y >= min1.y)
		penY = min(max1.y - min0.y, max0.y - min1.y);
	if (penetrationExists && min0.z <= max1.z && max0.z >= min1.z)
		penZ = min(max1.z - min0.z, max0.z - min1.z);

	if (penX <= penY && penX <= penZ)
	{
		if (max1.x - min0.x <= max0.x - min1.x)
			_contactN = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			_contactN = glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	else if (penY <= penZ && penY <= penX)
	{
		if (max1.y - min0.y <= max0.y - min1.y)
			_contactN = glm::vec3(0.0f, 1.0f, 0.0f);
		else 
			_contactN = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	else if (penZ <= penX && penZ <= penY)
	{
		if (max1.z - min0.z <= max0.z - min1.z)
			_contactN = glm::vec3(0.0f, 0.0f, 1.0f);
		else
			_contactN = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	if (penX == 0 && penY == 0 && penZ == 0)
		_t = 0.0f;
	else if (penetrationExists)
	{
		_t = -min(penX, min(penY, penZ));
	}
	else {
		_t = 1.0f;
	}

	return penetrationExists;
}

/** Collision resolution scheme based on the paper:  Iterative Dynamics with Temporal Coherence */
void resolveCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 _contactNormal, float _penetration, float _timestep)
{
	glm::vec3 rA = glm::normalize(_object2.getPosition() - _object1.getPosition());
	glm::vec3 rB = -rA;
	float JV = -glm::dot(_contactNormal, _object1.getVelocity()) - glm::dot(glm::cross(-rA, _contactNormal), _object1.getAngularVelocity())
		+ glm::dot(_contactNormal, _object2.getVelocity()) + glm::dot(glm::cross(-rB, _contactNormal), _object2.getAngularVelocity());

	// Determines the mass and moment of inertia matrices of our objects
	float r1 = _object1.getHitboxRadius();
	float m1;
	if (r1 == 0.125f)
		m1 = 1.0f;
	else
		m1 = 1000.0f;
	float M1Vals[9] = { m1,  0,  0,
						0, m1,  0,
						0,  0, m1 };
	glm::mat3 M1;
	memcpy(glm::value_ptr(M1), M1Vals, sizeof(M1Vals));

	float r2 = _object1.getHitboxRadius();
	float m2;
	if (r2 == 0.125f)
		m2 = 1.0f;
	else
		m2 = 1000.0f;
	float M2Vals[9] = { m2,  0,  0,
						0, m2,  0,
						0,  0, m2 };
	glm::mat3 M2;
	memcpy(glm::value_ptr(M2), M2Vals, sizeof(M2Vals));

	float i1 = 300 * m1 * r1 * r1 / 6;
	float I1Vals[9] = { i1,  0,  0,
					   0, i1,  0,
					   0,  0, i1 };
	glm::mat3 I1;
	memcpy(glm::value_ptr(I1), I1Vals, sizeof(I1Vals));

	float i2 = 300 * m2 * r2 * r2 / 6;
	float I2Vals[9] = { i2,  0,  0,
						0, i2,  0,
						0,  0, i2 };
	glm::mat3 I2;
	memcpy(glm::value_ptr(I2), I2Vals, sizeof(I2Vals));

	float Meff = glm::dot(-_contactNormal, glm::inverse(M1) * -_contactNormal) + 
		glm::dot(-glm::cross(-_contactNormal, _contactNormal), glm::inverse(I1) * -glm::cross(-_contactNormal, _contactNormal)) + 
		glm::dot(_contactNormal, glm::inverse(M2) * _contactNormal) + 
		glm::dot(glm::cross(-_contactNormal, _contactNormal), glm::inverse(I2) * glm::cross(-_contactNormal, _contactNormal));

	float lambda = -(JV + 0.10 / _timestep * _penetration) / Meff;

	glm::vec3 deltaV1 = M1 * lambda * -_contactNormal;
	glm::vec3 deltaW1 = I1 * lambda * -glm::cross(-rA, _contactNormal);
	glm::vec3 deltaV2 = M2 * lambda * _contactNormal;
	glm::vec3 deltaW2 = I2 * lambda * glm::cross(-rB, _contactNormal);
	cout << deltaW2.x << " " << deltaW2.y << " " << deltaW2.z << endl;

	// Apply impulses
	_object1.setVelocity(_object1.getVelocity() + deltaV1);
	_object1.setAngularVelocity(_object1.getAngularVelocity() + deltaW1);
	_object2.setVelocity(_object2.getVelocity() + deltaV2);
	_object2.setAngularVelocity(_object2.getAngularVelocity() + deltaW2);
}