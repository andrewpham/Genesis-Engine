// GL includes
#include <gengine/Model.h>
#include "RigidBodySim.h"

genesis::InputManager _physicsSimInputManager;
genesis::ResourceManager _physicsSimResourceManager;

/** For this demo, I implemented 2 different collision detection functions */
void getAxes(genesis::GameObject3D&, genesis::GameObject3D&, vector<glm::vec3>&);
glm::vec3 getAxisX(genesis::GameObject3D&, glm::mat3&);
glm::vec3 getAxisY(genesis::GameObject3D&, glm::mat3&);
glm::vec3 getAxisZ(genesis::GameObject3D&, glm::mat3&);
float transformToAxis(genesis::GameObject3D&, glm::vec3&);
float penetrationOnAxis(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&);
void fillPointFaceBoxBox(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, vector<glm::vec3>&, size_t);
bool checkCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);

bool checkAABBCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);

void computeBasis(const glm::vec3&, glm::vec3&, glm::vec3&);
void resolveCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3, float, float);

void run_physics_demo(GLFWwindow* window)
{
	// Setup and compile our shaders
	genesis::Shader shader = _physicsSimResourceManager.loadShader("Shaders/Rigid Body Sim/object.vs", "Shaders/Rigid Body Sim/object.frag", "shader");
	genesis::Shader skyboxShader = _physicsSimResourceManager.loadShader("../Genesis/Shaders/Life of Gaben/skybox.vs", "Shaders/Life of Gaben/skybox.frag", "skybox");

	// Skybox data and state
	GLuint skyboxVAO, skyboxVBO;

	struct
	{
		struct
		{
			GLint view;
			GLint projection;
			GLint lightPos;
			GLint viewPos;
		} object;
		struct
		{
			GLint view;
			GLint projection;
		} skybox;
	} uniforms;

	// Cache the uniform locations
	shader.Use();
	uniforms.object.view = glGetUniformLocation(shader.ID, "view");
	uniforms.object.projection = glGetUniformLocation(shader.ID, "projection");
	uniforms.object.lightPos = glGetUniformLocation(shader.ID, "lightPos");
	uniforms.object.viewPos = glGetUniformLocation(shader.ID, "viewPos");
	// Set the light source properties in the fragment shader
	glUniform3f(uniforms.object.lightPos, LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z);
	skyboxShader.Use();
	uniforms.skybox.view = glGetUniformLocation(skyboxShader.ID, "view");
	uniforms.skybox.projection = glGetUniformLocation(skyboxShader.ID, "projection");

	// Used to pace the spawning of objects at regular intervals
	GLfloat attackCooldown = 0.0f;

	/** Setup skybox VAO */
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), &SKYBOX_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("../Genesis/Textures/Skybox/Rigid Body Sim/right.png");
	faces.push_back("../Genesis/Textures/Skybox/Rigid Body Sim/left.png");
	faces.push_back("../Genesis/Textures/Skybox/Rigid Body Sim/top.png");
	faces.push_back("../Genesis/Textures/Skybox/Rigid Body Sim/bottom.png");
	faces.push_back("../Genesis/Textures/Skybox/Rigid Body Sim/back.png");
	faces.push_back("../Genesis/Textures/Skybox/Rigid Body Sim/front.png");
	_physicsSimResourceManager.loadCubemap(faces);
	GLuint cubemapTexture = _physicsSimResourceManager.getCubemap();

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
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.0f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.5f, -0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 6.0f, -0.875f)));
	//// Bottom left spawn
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 1.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 2.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 3.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 4.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 5.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(-0.875, 6.0f, 0.875f)));
	//// Bottom right spawn
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 1.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 2.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 2.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 3.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 3.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 4.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 4.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 5.0f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 5.5f, 0.875f)));
	//boxObjects.push_back(genesis::GameObject3D(shader, box, glm::vec3(0.875, 6.0f, 0.875f)));

	// Set the hitbox radii and normals
	for (genesis::GameObject3D &boxObject : boxObjects)
	{
		boxObject.setHitboxRadius(0.125f);
		boxObject.setIsStatic(false);
	}
	boxObjects[0].setHitboxRadius(1.0f);
	boxObjects[0].setIsStatic(true);

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
			boxObjects.back().setVelocity(7.0f * _physicsSimInputManager._camera.Front);
			boxObjects.back().setHitboxRadius(0.125f);
			boxObjects.back().setIsStatic(false);
			attackCooldown = 0.5f;
		}

		// Set the view position property in the fragment shader
		shader.Use();
		glUniform3f(uniforms.object.viewPos, _physicsSimInputManager._camera.Position.x, _physicsSimInputManager._camera.Position.y, _physicsSimInputManager._camera.Position.z);

		// Draw skybox first
		glDepthMask(GL_FALSE); // Remember to turn depth writing off

		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(_physicsSimInputManager._camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(_physicsSimInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(uniforms.skybox.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniforms.skybox.projection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE); // Turn depth writing back on

							  // Then draw scene as normal
		shader.Use();
		view = _physicsSimInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.object.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniforms.object.projection, 1, GL_FALSE, glm::value_ptr(projection));

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
				/** Forgo collision detection if we know that the boxes cannot possibly collide */
				if (i == j)
					continue;
				if (i != 0 && j != 0 && fabs(glm::length(boxObjects[i].getPosition() - boxObjects[j].getPosition())) > 0.434)
					continue;
				if (i == 0 || j == 0 && fabs(glm::length(boxObjects[i].getPosition() - boxObjects[j].getPosition())) > 1.95)
					continue;

				if (i == 0 || j == 0)
					collisionExists = checkAABBCollision(boxObjects[i], boxObjects[j], N, t);
				else
					collisionExists = checkCollision(boxObjects[i], boxObjects[j], N, t);
				if (collisionExists && t < 0)
				{
					resolveCollision(boxObjects[i], boxObjects[j], -N, t, _physicsSimInputManager.getDeltaTime());
				}
			}
		}

		// Draw the boxes
		for (genesis::GameObject3D &boxObject : boxObjects)
		{
			boxObject.render();
			// Update step
			if (!boxObject.getIsStatic())
				boxObject.setVelocity(boxObject.getVelocity() + G_CONST * _physicsSimInputManager.getDeltaTime());
			// Unless we're dealing with the big box
			else
			{
				boxObject.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
				boxObject.setAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			}
			boxObject.setPosition(boxObject.getPosition() + boxObject.getVelocity() * _physicsSimInputManager.getDeltaTime());
			boxObject.setAngularDisplacement(boxObject.getAngularDisplacement() + boxObject.getAngularVelocity() * _physicsSimInputManager.getDeltaTime());
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

/** Calculate the normals that need to be considered in collision detection */
void getAxes(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, vector<glm::vec3> &_axes)
{
	glm::mat4 rotationMat;
	rotationMat = glm::rotate(rotationMat, _object1.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, 1.0f));
	rotationMat = glm::rotate(rotationMat, _object1.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object1.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 rotation = glm::mat3(rotationMat);
	glm::vec3 a0 = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 a1 = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 a2 = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	rotationMat = glm::mat4();
	rotationMat = glm::rotate(rotationMat, _object2.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, 1.0f));
	rotationMat = glm::rotate(rotationMat, _object2.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object2.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::mat3(rotationMat);
	glm::vec3 b0 = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 b1 = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 b2 = rotation * glm::vec3(0.0f, 0.0f, 1.0f);

	_axes.push_back(a0);
	_axes.push_back(a1);
	_axes.push_back(a2);
	_axes.push_back(b0);
	_axes.push_back(b1);
	_axes.push_back(b2);
	_axes.push_back(glm::cross(a0, b0));
	_axes.push_back(glm::cross(a0, b1));
	_axes.push_back(glm::cross(a0, b2));
	_axes.push_back(glm::cross(a1, b0));
	_axes.push_back(glm::cross(a1, b1));
	_axes.push_back(glm::cross(a1, b2));
	_axes.push_back(glm::cross(a2, b0));
	_axes.push_back(glm::cross(a2, b1));
	_axes.push_back(glm::cross(a2, b2));
}

glm::vec3 getAxisX(genesis::GameObject3D &_object, glm::mat3 &_rotation)
{
	return _rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 getAxisY(genesis::GameObject3D &_object, glm::mat3 &_rotation)
{
	return _rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 getAxisZ(genesis::GameObject3D &_object, glm::mat3 &_rotation)
{
	return _rotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

float transformToAxis(genesis::GameObject3D &_object, glm::vec3 &_axis)
{
	glm::mat4 rotationMat;
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, 1.0f));
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 rotation = glm::mat3(rotationMat);

	return _object.getHitboxRadius() * fabs(glm::dot(_axis, getAxisX(_object, rotation))) +
		_object.getHitboxRadius() * fabs(glm::dot(_axis, getAxisY(_object, rotation))) +
		_object.getHitboxRadius() * fabs(glm::dot(_axis, getAxisZ(_object, rotation)));
}

float penetrationOnAxis(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_axis)
{
	float oneProject = transformToAxis(_object1, _axis);
	float twoProject = transformToAxis(_object2, _axis);

	float distance = fabs(glm::dot(_object2.getPosition() - _object1.getPosition(), _axis));

	// Return the overlap (i.e., positive indicates
	// overlap, negative indicates separation).
	return oneProject + twoProject - distance;
}

void fillPointFaceBoxBox(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_contactN, vector<glm::vec3> &_axes, size_t best)
{
	glm::vec3 normal = _axes[best];
	// Which face is in contact?
	if (glm::dot(normal, _object2.getPosition() - _object1.getPosition()) > 0)
		normal *= -1.0f;

	_contactN = glm::normalize(normal);
}

/** SAT collision detection scheme based on ideas from the book:  Game Physics Engine Development */
bool checkCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_contactN, float &_t)
{
	vector<glm::vec3> axes;
	getAxes(_object1, _object2, axes);

	float bestOverlap = std::numeric_limits<float>::max();
	size_t bestCase;
	size_t i;
	for (i = 0; i < axes.size(); i++)
	{
		glm::vec3 axis = axes[i];

		// Check for axes that were generated by (almost) parallel edges.
		if (glm::length(axis) < 0.001) continue;
		axis = glm::normalize(axis);

		float overlap = penetrationOnAxis(_object1, _object2, axis);
		if (overlap < 0) return false;
		if (overlap < bestOverlap)
		{
			bestOverlap = overlap;
			bestCase = i;
		}
	}

	if (bestCase < 3)
	{
		fillPointFaceBoxBox(_object1, _object2, _contactN, axes, bestCase);
		_t = -bestOverlap;
		return true;
	}
	else if (bestCase < 6)
	{
		fillPointFaceBoxBox(_object2, _object1, _contactN, axes, bestCase);
		_t = -bestOverlap;
		return true;
	}
	else
	{
		glm::vec3 axis = axes[bestCase];
		axis = glm::normalize(axis);

		if (glm::dot(axis, _object2.getPosition() - _object1.getPosition()) > 0) axis *= -1.0f;

		_contactN = axis;
		_t = -bestOverlap;
		return true;
	}
	return false;
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

void computeBasis(const glm::vec3 &n, glm::vec3 &t1, glm::vec3 &t2)
{
	// Suppose vector a has all equal components and is a unit vector: a = (s, s, s)
	// Then 3*s*s = 1, s = sqrt(1/3) = 0.57735. This means that at least one component of a
	// unit vector must be greater or equal to 0.57735.

	if (fabs(n.x) >= 0.57735f)
		t1 = glm::vec3(n.y, -n.x, 0.0f);
	else
		t1 = glm::vec3(0.0f, n.z, -n.y);

	t1 = glm::normalize(t1);
	t2 = glm::normalize(glm::cross(n, t1));
}

/** Collision resolution scheme based on the paper:  Iterative Dynamics with Temporal Coherence */
void resolveCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 _contactNormal, float _penetration, float _timestep)
{
	float totalImpulseN = 0.0f;
	float totalImpulseT1 = 0.0f;
	float totalImpulseT2 = 0.0f;
	glm::vec3 t1, t2;
	computeBasis(_contactNormal, t1, t2);

	int i;
	for (i = 0; i < 3; i++)
	{
		glm::vec3 rA = glm::normalize(_object2.getPosition() - _object1.getPosition());
		glm::vec3 rB = -rA;
		float JnV = -glm::dot(_contactNormal, _object1.getVelocity()) - glm::dot(glm::cross(-rA, _contactNormal), _object1.getAngularVelocity())
			+ glm::dot(_contactNormal, _object2.getVelocity()) + glm::dot(glm::cross(-rB, _contactNormal), _object2.getAngularVelocity());

		// Determines the mass and moment of inertia matrices of our objects
		float r1 = _object1.getHitboxRadius();
		float m1;
		if (r1 == 0.125f)
			m1 = 1.0f;
		else
			m1 = 512.0f;
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
			m2 = 512.0f;
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

		float MeffN = glm::dot(-_contactNormal, glm::inverse(M1) * -_contactNormal) +
			glm::dot(-glm::cross(-rA, _contactNormal), glm::inverse(I1) * -glm::cross(-rA, _contactNormal)) +
			glm::dot(_contactNormal, glm::inverse(M2) * _contactNormal) +
			glm::dot(glm::cross(-rB, _contactNormal), glm::inverse(I2) * glm::cross(-rB, _contactNormal));

		float lambdaN = -(JnV + 0.10f / _timestep * _penetration) / MeffN;
		float oldImpulseN = totalImpulseN;
		totalImpulseN = glm::clamp(lambdaN + oldImpulseN, 0.0f, std::numeric_limits<float>::max());
		lambdaN = totalImpulseN - oldImpulseN;

		glm::vec3 deltaV1 = M1 * lambdaN * -_contactNormal;
		glm::vec3 deltaW1 = I1 * lambdaN * -glm::cross(-rA, _contactNormal);
		glm::vec3 deltaV2 = M2 * lambdaN * _contactNormal;
		glm::vec3 deltaW2 = I2 * lambdaN * glm::cross(-rB, _contactNormal);

		// Apply impulses
		_object1.setVelocity(_object1.getVelocity() + deltaV1);
		_object1.setAngularVelocity(_object1.getAngularVelocity() + deltaW1);
		_object2.setVelocity(_object2.getVelocity() + deltaV2);
		_object2.setAngularVelocity(_object2.getAngularVelocity() + deltaW2);

		/** Now deal with the friction constraints */
		float Jt1V = -glm::dot(t1, _object1.getVelocity()) - glm::dot(glm::cross(-rA, t1), _object1.getAngularVelocity())
			+ glm::dot(t1, _object2.getVelocity()) + glm::dot(glm::cross(-rB, t1), _object2.getAngularVelocity());

		float MeffT1 = glm::dot(-t1, glm::inverse(M1) * -t1) +
			glm::dot(-glm::cross(-rA, t1), glm::inverse(I1) * -glm::cross(-rA, t1)) +
			glm::dot(t1, glm::inverse(M2) * t1) +
			glm::dot(glm::cross(-rB, t1), glm::inverse(I2) * glm::cross(-rB, t1));

		float lambdaT1 = -Jt1V / MeffT1;
		float oldImpulseT1 = totalImpulseT1;
		float limitT1 = 0.65f * totalImpulseN;
		totalImpulseT1 = glm::clamp(lambdaT1 + oldImpulseT1, -limitT1, limitT1);
		lambdaT1 = totalImpulseT1 - oldImpulseT1;

		deltaV1 = M1 * lambdaT1 * -t1;
		deltaW1 = I1 * lambdaT1 * -glm::cross(-rA, t1);
		deltaV2 = M2 * lambdaT1 * t1;
		deltaW2 = I2 * lambdaT1 * glm::cross(-rB, t1);

		// Apply impulses
		_object1.setVelocity(_object1.getVelocity() + deltaV1);
		_object1.setAngularVelocity(_object1.getAngularVelocity() + deltaW1);
		_object2.setVelocity(_object2.getVelocity() + deltaV2);
		_object2.setAngularVelocity(_object2.getAngularVelocity() + deltaW2);

		float Jt2V = -glm::dot(t2, _object2.getVelocity()) - glm::dot(glm::cross(-rA, t2), _object2.getAngularVelocity())
			+ glm::dot(t2, _object2.getVelocity()) + glm::dot(glm::cross(-rB, t2), _object2.getAngularVelocity());

		float MeffT2 = glm::dot(-t2, glm::inverse(M1) * -t2) +
			glm::dot(-glm::cross(-rA, t2), glm::inverse(I1) * -glm::cross(-rA, t2)) +
			glm::dot(t2, glm::inverse(M2) * t2) +
			glm::dot(glm::cross(-rB, t2), glm::inverse(I2) * glm::cross(-rB, t2));

		float lambdaT2 = -Jt2V / MeffT2;
		float oldImpulseT2 = totalImpulseT2;
		float limitT2 = 0.65f * totalImpulseN;
		totalImpulseT2 = glm::clamp(lambdaT2 + oldImpulseT2, -limitT2, limitT2);
		lambdaT2 = totalImpulseT2 - oldImpulseT2;

		deltaV1 = M1 * lambdaT2 * -t2;
		deltaW1 = I1 * lambdaT2 * -glm::cross(-rA, t2);
		deltaV2 = M2 * lambdaT2 * t2;
		deltaW2 = I2 * lambdaT2 * glm::cross(-rB, t2);

		// Apply impulses
		_object2.setVelocity(_object2.getVelocity() + deltaV1);
		_object2.setAngularVelocity(_object2.getAngularVelocity() + deltaW1);
		_object2.setVelocity(_object2.getVelocity() + deltaV2);
		_object2.setAngularVelocity(_object2.getAngularVelocity() + deltaW2);
	}
}