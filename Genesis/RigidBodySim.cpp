// GL includes
#include <gengine/Model.h>
#include "RigidBodySim.h"

genesis::InputManager _physicsSimInputManager;
genesis::ResourceManager _physicsSimResourceManager;
std::vector<glm::vec3> _normals { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f),
									glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f) };

/** Function descriptions can be found next to the corresponding definitions below */
/** For this demo, I implemented 3 different collision detection functions */
void getAxes(genesis::GameObject3D&, genesis::GameObject3D&, vector<glm::vec3>&);
void getVertices(genesis::GameObject3D&, vector<glm::vec3>&);
bool intersectsWhenProjected(vector<glm::vec3>&, vector<glm::vec3>&, glm::vec3&, float&);
bool checkSATCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);

glm::vec3 getFarthestPointInDirection(vector<glm::vec3>&, glm::vec3&);
glm::vec3 support(vector<glm::vec3>&, vector<glm::vec3>&, glm::vec3&);
bool update(vector<glm::vec3>&, glm::vec3&, glm::vec3&);
void findClosestPointInSimplex(vector<glm::vec3>&, glm::vec3&);
bool checkGJKCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);

bool checkAABBCollision(genesis::GameObject3D&, genesis::GameObject3D&, glm::vec3&, float&);

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
		boxObject.setNormals(_normals);
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
			boxObjects.back().setVelocity(5.0f * _physicsSimInputManager._camera.Front);
			boxObjects.back().setHitboxRadius(0.125f);
			boxObjects.back().setNormals(_normals);
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
				if (i != 0 && j != 0 && fabs(glm::length(boxObjects[i].getPosition() - boxObjects[j].getPosition())) > 0.6)
					continue;
				if (i == 0 || j == 0 && fabs(glm::length(boxObjects[i].getPosition() - boxObjects[j].getPosition())) > 2)
					continue;

				collisionExists = checkAABBCollision(boxObjects[i], boxObjects[j], N, t);
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
	rotationMat = glm::rotate(rotationMat, _object1.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, -1.0f));
	rotationMat = glm::rotate(rotationMat, _object1.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object1.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 rotation = glm::mat3(rotationMat);
	glm::vec3 a0 = _object1.getPosition() + rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 a1 = _object1.getPosition() + rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 a2 = _object1.getPosition() + rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	rotationMat = glm::mat4();
	rotationMat = glm::rotate(rotationMat, _object2.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, -1.0f));
	rotationMat = glm::rotate(rotationMat, _object2.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object2.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::mat3(rotationMat);
	glm::vec3 b0 = _object2.getPosition() + rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 b1 = _object2.getPosition() + rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 b2 = _object2.getPosition() + rotation * glm::vec3(0.0f, 0.0f, 1.0f);

	_axes.push_back(glm::normalize(a0));
	_axes.push_back(glm::normalize(a1));
	_axes.push_back(glm::normalize(a2));
	_axes.push_back(glm::normalize(b0));
	_axes.push_back(glm::normalize(b1));
	_axes.push_back(glm::normalize(b2));
	_axes.push_back(glm::normalize(glm::cross(a0, b0)));
	_axes.push_back(glm::normalize(glm::cross(a0, b1)));
	_axes.push_back(glm::normalize(glm::cross(a0, b2)));
	_axes.push_back(glm::normalize(glm::cross(a1, b0)));
	_axes.push_back(glm::normalize(glm::cross(a1, b1)));
	_axes.push_back(glm::normalize(glm::cross(a1, b2)));
	_axes.push_back(glm::normalize(glm::cross(a2, b0)));
	_axes.push_back(glm::normalize(glm::cross(a2, b1)));
	_axes.push_back(glm::normalize(glm::cross(a2, b2)));
}

void getVertices(genesis::GameObject3D &_object, vector<glm::vec3> &_vertices)
{
	float r = _object.getHitboxRadius();
	glm::mat4 rotationMat;
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().y, glm::vec3(0.0f, 0.0f, -1.0f));
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().x, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMat = glm::rotate(rotationMat, _object.getAngularDisplacement().z, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 rotation = glm::mat3(rotationMat);
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, -r, -r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, -r, -r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, r, -r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, -r, r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, r, -r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, -r, r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(-r, r, r));
	_vertices.push_back(_object.getPosition() + rotation * glm::vec3(r, r, r));
}

bool intersectsWhenProjected(vector<glm::vec3> &_vertices1, vector<glm::vec3> &_vertices2, glm::vec3 &_axis, float &_t)
{
	// Handles the case where the cross product = {0, 0, 0}
	if (fabs(_axis.x) < 0.001f && fabs(_axis.y) < 0.001f && fabs(_axis.z) < 0.001f)
		return true;

	float aMin = std::numeric_limits<float>::max();
	float aMax = std::numeric_limits<float>::min();
	float bMin = std::numeric_limits<float>::max();
	float bMax = std::numeric_limits<float>::min();

	// Determine the two projected intervals
	int i;
	for (i = 0; i < _vertices1.size(); i++)
	{
		float aDist = glm::dot(_vertices1[i], _axis);
		aMin = (aDist < aMin) ? aDist : aMin;
		aMax = (aDist > aMax) ? aDist : aMax;
		float bDist = glm::dot(_vertices2[i], _axis);
		bMin = (bDist < bMin) ? bDist : bMin;
		bMax = (bDist > bMax) ? bDist : bMax;
	}

	// Intersection test
	float longSpan = max(aMax, bMax) - min(aMin, bMin);
	float sumSpan = aMax - aMin + bMax - bMin;
	_t = longSpan - sumSpan;
	return longSpan < sumSpan;
}

/** Checks the two rigid bodies to see if a collision occurs, and calculates the contact normal and collision type descriptor (t) */
bool checkSATCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_contactN, float &_t)
{
	vector<glm::vec3> axes;
	getAxes(_object1, _object2, axes);
	
	vector<glm::vec3> vertices1, vertices2;
	getVertices(_object1, vertices1);
	getVertices(_object2, vertices2);

	int i;
	float pen;
	_t = std::numeric_limits<float>::max();
	for (i = 0; i < axes.size(); i++)
	{
		if (!intersectsWhenProjected(vertices1, vertices2, axes[i], pen))
		{
			return false;
		}
		if (pen < _t)
		{
			_t = pen;
			_contactN = axes[i];
		}
	}

	if (_t < 0)
	{
		if (glm::dot(_contactN, _object2.getPosition() - _object1.getPosition()) < 0)
			_contactN = -_contactN;
		return true;
	}
	else
		return false;
}

glm::vec3 getFarthestPointInDirection(vector<glm::vec3> &_vertices, glm::vec3 &_d)
{
	glm::vec3 farthest = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	for (glm::vec3 vertex : _vertices)
	{
		if (glm::dot(vertex, _d) > glm::dot(farthest, _d))
			farthest = vertex;
	}
	return farthest;
}

glm::vec3 support(vector<glm::vec3> &_vertices1, vector<glm::vec3> &_vertices2, glm::vec3 &_d)
{
	// d is a vector direction (doesn't have to be normalized)
	// Get points on the edge of the shapes in opposite directions
	glm::vec3 p1 = getFarthestPointInDirection(_vertices1, _d);
	glm::vec3 p2 = getFarthestPointInDirection(_vertices2, -_d);
	return p1 - p2;
}

bool update(vector<glm::vec3> &_simplex, glm::vec3 &a, glm::vec3 &_d)
{
	if (_simplex.size() == 0)
	{
		_simplex.push_back(a);
		_d = -a;
		
		// Can't enclose anything with just a single point
		return false;
	}

	if (_simplex.size() == 1)
	{
		glm::vec3 b = _simplex[0];
		glm::vec3 ab = b - a;
		glm::vec3 ao = -a;
		_simplex.push_back(a);
		_d = glm::cross(glm::cross(ab, ao), ab);

		// Can't enclose the origin without a tetrahedron
		return false;
	}

	if (_simplex.size() == 2)
	{
		glm::vec3 ao = -a;

		glm::vec3 b = _simplex[1];
		glm::vec3 c = _simplex[0];

		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;

		// Triangle's normal
		glm::vec3 abc = glm::cross(ab, ac);

		// The vector within the plane of the triangle pointing away from the edge AB
		glm::vec3 abPerp = glm::cross(ab, abc);

		if (glm::dot(abPerp, ao) > 0)
		{
			_simplex.erase(_simplex.begin());
			_simplex.push_back(a);
			_d = glm::cross(glm::cross(ab, ao), ab);
			return false;
		}

		// Now consider the edge AC
		glm::vec3 acPerp = glm::cross(abc, ac);
		if (glm::dot(acPerp, ao) > 0)
		{
			_simplex.pop_back();
			_simplex.push_back(a);
			_d = glm::cross(glm::cross(ac, ao), ac);
			return false;
		}

		// The origin is inside the triangle; test to see if it is above or below it
		if (glm::dot(abc, ao) > 0)
		{
			_simplex.push_back(a);
			_d = abc;
		}
		else
		{
			_simplex[0] = b;
			_simplex[1] = c;
			_simplex.push_back(a);
			_d = -abc;
		}

		return false;
	}

	if (_simplex.size() == 3)
	{
		glm::vec3 ao = -a;

		glm::vec3 b = _simplex[2];
		glm::vec3 c = _simplex[1];
		glm::vec3 d = _simplex[0];

		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		glm::vec3 ad = d - a;

		glm::vec3 abc = glm::cross(ab, ac);
		glm::vec3 acd = glm::cross(ac, ad);
		glm::vec3 adb = glm::cross(ad, ab);

		glm::vec3 tmp;

		const int over_abc = 0x1;
		const int over_acd = 0x2;
		const int over_adb = 0x4;

		int plane_tests = 
			(glm::dot(abc, ao) > 0 ? over_abc : 0) |
			(glm::dot(acd, ao) > 0 ? over_acd : 0) |
			(glm::dot(adb, ao) > 0 ? over_adb : 0);

		switch (plane_tests)
		{
		case 0:
			// Behind all three faces, thus the tet encloses the origin
			return true;

		case over_abc:
			goto check_one_face;

		case over_acd:
			// Rotate ACD onto ABC
			b = c;
			c = d;

			ab = ac;
			ac = ad;

			abc = acd;

			goto check_one_face;

		case over_adb:
			// Rotate ADB onto ABC
			c = b;
			b = d;

			ac = ab;
			ab = ad;

			abc = adb;

			goto check_one_face;

		case over_acd | over_adb:
			// Rotate ACD, ADB into ABC, ACD
			tmp = b;
			b = c;
			c = d;
			d = tmp;

			tmp = ab;
			ab = ac;
			ac = ad;
			ad = tmp;

			abc = acd;
			acd = adb;

			goto check_two_faces;

		case over_adb | over_abc:
			// Rotate ADB, ABC into ABC, ACD
			tmp = c;
			c = b;
			b = d;
			d = tmp;

			tmp = ac;
			ac = ab;
			ab = ad;
			ad = tmp;

			acd = abc;
			abc = adb;

			goto check_two_faces;

		default:
			return true;
		}

	check_one_face:

		if (dot(cross(abc, ac), ao) > 0)
		{
			// In the AC region
			_simplex.pop_back();
			_simplex.push_back(a);
			_simplex.erase(_simplex.begin());
			_d = glm::cross(glm::cross(ac, ao), ac);
			return false;
		}

	check_one_face_part_2:

		if (dot(cross(ab, abc), ao) > 0)
		{
			// In the AB region
			_simplex[1] = b;
			_simplex[2] = a;
			_simplex.erase(_simplex.begin());
			_d = glm::cross(glm::cross(ab, ao), ab);
			return false;
		}

		// In the ABC region
		_simplex[0] = c;
		_simplex[1] = b;
		_simplex[2] = a;
		_d = abc;
		return false;

	check_two_faces:

		if (dot(cross(abc, ac), ao) > 0)
		{
			b = c;
			c = d;

			ab = ac;
			ac = ad;

			abc = acd;

			goto check_one_face;
		}

		goto check_one_face_part_2;
	}

	return false;
}

/** EPA subroutine.  Source at:  http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=4&t=8243 */
void findClosestPointInSimplex(vector<glm::vec3> &_simplexPoints, glm::vec3 &_outContactPoint)
{
	float barycentricCoord0 = 0.0;
	float barycentricCoord1 = 0.0;
	float barycentricCoord2 = 0.0;
	float barycentricCoord3 = 0.0;

	switch (_simplexPoints.size())
	{
	case 1:
	{
		// The point on the simplex closest to the origin is the point itself
		_outContactPoint = _simplexPoints[0];
		barycentricCoord0 = 1.0;
	}
	break;

	case 2:
	{
		// We have a line
		glm::vec3 &start = _simplexPoints[0];
		glm::vec3 &end = _simplexPoints[1];

		glm::vec3 endToOrigin = end;
		endToOrigin *= -1.0;

		glm::vec3 endToStart = start;
		endToStart -= end;

		// The parametric value for the intersection is (p-q).u / u.u
		// We can check for zero/negative without the denominator, so let's do so first
		float numerator = glm::dot(endToOrigin, endToStart);
		if (numerator <= 0.0)
		{
			// it's closer to the end point, so remove the start point
			_outContactPoint = _simplexPoints[1];
			_simplexPoints.erase(_simplexPoints.begin());
			barycentricCoord0 = 1.0;
			break;
		}

		// Check for the other side
		float denominator = glm::dot(endToStart, endToStart);
		if (denominator <= numerator)
		{
			// It's closer to the start point, so remove the end point
			_outContactPoint = _simplexPoints[0];
			_simplexPoints.pop_back();
			barycentricCoord0 = 1.0;
			break;
		}

		// The point on the simplex closest to the origin is on the line segment
		float t = numerator / denominator;
		_outContactPoint = endToStart;
		_outContactPoint *= t;
		_outContactPoint += end;
		barycentricCoord0 = t;
		barycentricCoord1 = 1.0f - t;
	}
	break;

	case 3:
	{
		glm::vec3 &a = _simplexPoints[0];
		glm::vec3 &b = _simplexPoints[1];
		glm::vec3 &c = _simplexPoints[2];
		glm::vec3 &originToa = a;
		glm::vec3 &originTob = b;
		glm::vec3 &originToc = c;

		float originToaDotaTob;
		float aTobDotOriginTob;
		float cTobDotOriginToc;

		glm::vec3 aTob = b;
		aTob -= a;
		glm::vec3 cTob = b;
		cTob -= c;
		glm::vec3 cToa = a;
		cToa -= c;

		float aTobDotaTob = glm::length(aTob);
		float cTobDotcTob = glm::length(cTob);
		float cToaDotcToa = glm::length(cToa);

		// Take care of degeneracy
		if (aTobDotaTob < 0.00001)
		{
			vector<glm::vec3> simplex;
			simplex.push_back(_simplexPoints[0]);
			simplex.push_back(_simplexPoints[2]);
			findClosestPointInSimplex(simplex, _outContactPoint);
			_simplexPoints = simplex;
			break;
		}
		if (cTobDotcTob < 0.00001 || cToaDotcToa < 0.00001)
		{
			vector<glm::vec3> simplex;
			simplex.push_back(_simplexPoints[0]);
			simplex.push_back(_simplexPoints[1]);
			findClosestPointInSimplex(simplex, _outContactPoint);
			_simplexPoints = simplex;
			break;
		}

		// Vertex A

		originToaDotaTob = glm::dot(originToa, aTob);

		if (originToaDotaTob >= 0.0)
		{
			if (glm::dot(originToa, cToa) <= 0.0)
			{
				// a is closest point, remove the rest
				_outContactPoint = _simplexPoints[0];
				_simplexPoints.pop_back();
				_simplexPoints.pop_back();
				barycentricCoord0 = 1.0;
				break;
			}
		}

		glm::vec3 normal = cTob;
		normal = glm::cross(normal, aTob);

		glm::vec3 originTobDir = originTob;
		originTobDir = glm::normalize(originTobDir);

		// Edge BA
		// If we got here, we can assume that originToa.DotProductWith(aTob) < 0.0                          
		glm::vec3 negbaNorm = normal;
		negbaNorm = glm::cross(negbaNorm, aTob);
		float normMag = glm::length(negbaNorm);
		float normDP;

		aTobDotaTob = glm::length(aTob);
		aTobDotOriginTob = glm::dot(aTob, originTob);

		if (normMag > 0.00001)
		{
			negbaNorm /= normMag;

			normDP = glm::dot(negbaNorm, originTobDir);
			if (fabs(normDP) < 0.00001)
				normDP = 0.0;
		}
		else
			normDP = 0.0;

		if (normDP >= 0.0)
		{
			if (aTobDotOriginTob >= 0.0)
			{
				// If we go into the "distance squared space" we can avoid some division operations
				// We proceed by using the barycentric coordinates with the barycenter at b
				// Then we parameterize the triangle with:
				// T(s,t) = B + s*bToa + t*bToc
				// The distance squared formula from the origin to this triangle gives us:
				// Q(s,t) = a*s^2 + 2*b*s*t + c*t^2 + 2*d*s + 2*e*t + f, where:
				// a = bToa.bToa
				// b = bToa.bToc
				// c = bToc.bToc
				// d = bToa.originTob
				// e = bToc.originTob
				// f = originTob.originTob
				// We reduce the problem to one dimension using the following relation: t = 0 (in other words, we are looking for                     
				// the squared distance to the boundary of the triangle):
				// F(s) = Q(s,0) = a*s^2 + 2*d*s + f
				// Differentiating and equating to zero for the critical point, we get:
				// F'(s) = a*s + d = 0
				// s = -d / a = aTob.originTob / aTob.aTob

				float s = aTobDotOriginTob / aTobDotaTob;
				_outContactPoint = aTob;
				_outContactPoint *= -s;
				_outContactPoint += b;
				_simplexPoints.pop_back();
				// We want barycentric coordinates such that no additional math needs to be done to calculate the closest points as 
				// follows:
				// T'(x,y,z) = A*x + B*y + C*z
				// T' = B + (A-B)s + (C-B)t = B + As - Bs + Ct - Bt = B*(1-s-t) + As + Ct
				barycentricCoord0 = s;
				barycentricCoord1 = 1.0 - s;
				break;
			}
		}

		// Vertex B
		if (glm::dot(cTob, originTob) <= 0.0 && aTobDotOriginTob <= 0.0)
		{
			// b is closest point, remove the rest
			_simplexPoints.pop_back();
			_simplexPoints.erase(_simplexPoints.begin());
			barycentricCoord0 = 1.0;
			break;
		}

		// Edge CB
		glm::vec3 bcNorm = normal;
		bcNorm = glm::cross(bcNorm, cTob);
		normMag = glm::length(bcNorm);

		cTobDotcTob = glm::length(cTob);
		cTobDotOriginToc = glm::dot(cTob, originToc);

		if (normMag > 0.00001)
		{
			bcNorm /= normMag;

			// Make sure it's not in the interior of the triangle
			normDP = glm::dot(bcNorm, originTobDir);
			if (fabs(normDP) < 0.00001)
				normDP = 0.0;
		}
		else
			normDP = 0.0;

		if (normDP <= 0.0)
		{
			if (cTobDotOriginToc <= 0.0)
			{
				// T(s,t) = C + s*cTob + t*cToa
				// The distance squared formula from the origin to this triangle gives us:
				// Q(s,t) = a*s^2 + 2*b*s*t + c*t^2 + 2*d*s + 2*e*t + f, where:
				// a = cTob.cTob
				// b = cTob.cToa
				// c = cToa.cToa
				// d = cTob.originToc
				// e = cToa.originToc
				// f = originToc.originToc
				// We reduce the problem to one dimension using the following relation: t = 0 (in other words, we are looking for                     
				// the squared distance to the boundary of the triangle):
				// F(s) = Q(s,0) = a*s^2 + 2*d*s + f
				// Differentiating and equating to zero for the critical point, we get:
				// F'(s) = a*s + d = 0
				// s = -d / a = cTob.originToc / cTob.cTob                 

				float s = -cTobDotOriginToc / cTobDotcTob;
				_outContactPoint = cTob;
				_outContactPoint *= s;
				_outContactPoint += c;
				_simplexPoints.erase(_simplexPoints.begin());
				// Removed 0 (a), 0th element is c now, 1st element is b
				// We want barycentric coordinates such that no additional math needs to be done to calculate the closest points as:
				// T'(x,y,z) = A*x + B*y + C*z
				// T' = C + (B-C)s + (A-C)t = C + Bs - Cs + At - Ct = C*(1-s-t) + Bs + At
				barycentricCoord0 = 1.0 - s;
				barycentricCoord1 = s;
				break;
			}
		}

		// Vertex C             
		if (glm::dot(cToa, originToc) >= 0.0 && cTobDotOriginToc >= 0.0)
		{
			// c is closest point, remove the rest
			_outContactPoint = _simplexPoints[2];
			_simplexPoints.erase(_simplexPoints.begin() + 1);
			_simplexPoints.erase(_simplexPoints.begin());
			barycentricCoord0 = 1.0;
			break;
		}

		// Edge AC         
		// We just need to see if it's in the interior or not
		glm::vec3 negacNorm = normal;
		negacNorm = glm::cross(negacNorm, cToa);
		normMag = glm::length(negacNorm);

		if (normMag > 0.00001)
		{
			negacNorm /= normMag;

			glm::vec3 originTocDir = originToc;
			originTocDir = glm::normalize(originTocDir);

			normDP = glm::dot(negacNorm, originTocDir);
			if (fabs(normDP) < 0.00001)
				normDP = 0.0;
		}
		else
			normDP = 0.0;

		if (normDP >= 0.0)
		{
			// T(s,t) = C + s*cTob + t*cToa
			// We reduce the problem to one dimension using the following relation: s = 0 (in other words, we are looking for
			// the squared distance to the boundary of the triangle:
			// F(t) = Q(0,t) = 2*e*t + c*t^2 + f
			// Differentiating and equating to zero for the critical point, we get:
			// F'(t) = c*t + e = 0
			// t = -e / c = -cToa.originToc / cToa.cToa
			float t = -glm::dot(cToa, originToc) / cToaDotcToa;
			_outContactPoint = cToa;
			_outContactPoint *= t;
			_outContactPoint += c;
			// remove b
			_simplexPoints.erase(_simplexPoints.begin() + 1);
			// We want barycentric coordinates such that no additional math needs to be done to calculate the closest points:
			// T'(x,y,z) = A*x + B*y + C*z
			// T' = C + (B-C)s + (A-C)t = C + Bs - Cs + At - Ct = C*(1-s-t) + Bs + At
			barycentricCoord0 = t;
			barycentricCoord1 = 1.0 - t;
			break;
		}

		// If we got this far, it is in the interior of the triangle
		// We go into the "distance squared space" as before
		// We proceed by using barycentric coordinates with the barycenter at b
		// Then we parameterize the triangle with:
		// T(s,t) = B + s*bToa + t*bToc
		// The distance squared formula from the origin to this triangle gives us:
		// Q(s,t) = a*s^2 + 2*b*s*t + c*t^2 + 2*d*s + 2*e*t + f, where:
		// a = bToa.bToa
		// b = bToa.bToc
		// c = bToc.bToc
		// d = bToa.originTob
		// e = bToc.originTob
		// f = originTob.originTob
		// Differentiating and equating to zero for the critical points, we get:
		// Qs'(s,t) = 2*a*s + 2*b*t + 2*d = 0
		// Qs'(s,t) = a*s + b*t = -d
		// Qt'(s,t) = 2*b*s + 2*c*t + 2*e = 0
		// Qt'(s,t) = b*s + c*t = -e

		// s = (-d - b*t) / a
		// b*(-d - b*t)/a + c*t = -e
		// -b*d/a - b*b*t/a + c*t = -e
		// -b*d - b*b*t + a*c*t = -e*a
		// (a*c - b*b)*t = b*d - e*a
		// t = (b*d - e*a) / (a*c - b*b), note that the denominator is the normal magnitude squared

		// t = (-e - b*s) / c
		// a*s + b*(-e - b*s)/c = -d
		// a*c*s - b*e - b*b*s = -c*d
		// s * (a*c - b*b) = -c*d + b*e
		// s = (b*e - c*d) / (a*c - b*b), note that the denominator is the normal magnitude squared

		// s = (bToa.bToc * bToc.originTob - bToc.bToc * bToa.originTob) / normal.normal
		normMag = glm::length(normal);
		if (normMag < 0.00001)
			normMag = 0.00001;
		float invNormMagSquared = 1.0 / normMag;
		float cTobDotOriginTob = glm::dot(cTob, originTob);
		float aTobDotcTob = glm::dot(aTob, cTob);
		float negs = (aTobDotcTob * cTobDotOriginTob - cTobDotcTob * aTobDotOriginTob) * invNormMagSquared;

		// t = (bToa.bToc * bToa.originTob - bToc.originTob * bToa.bToa) / normal.normal
		float negt = (aTobDotcTob * aTobDotOriginTob - cTobDotOriginTob * aTobDotaTob) * invNormMagSquared;

		glm::vec3 tTerm = cTob;
		tTerm *= negt;

		_outContactPoint = aTob;
		_outContactPoint *= negs;
		_outContactPoint += tTerm;
		_outContactPoint += b;

		// We want barycentric coordinates such that no additional math needs to be done to calculate the closest points:
		// T'(x,y,z) = A*x + B*y + C*z
		// T' = B + (A-B)s + (C-B)t = B - Bs + As + Ct - Bt = B*(1-s-t) + As + Ct

		barycentricCoord0 = -negs;
		barycentricCoord1 = 1.0 + negs + negt;
		barycentricCoord2 = -negt;
	}
	break;

	case 4:
	{
		glm::vec3 &a = _simplexPoints[0];
		glm::vec3 &b = _simplexPoints[1];
		glm::vec3 &c = _simplexPoints[2];
		glm::vec3 &d = _simplexPoints[3];
		vector<vector<glm::vec3>> triangles;

		float closestDistSquared = std::numeric_limits<float>::max();
		int closestIdx = -1;

		glm::vec3 vec1 = c;
		vec1 -= b;
		glm::vec3 vec2 = a;
		vec2 -= b;
		glm::vec4 plane = glm::vec4(vec1, 1.0f);
		plane = glm::vec4(glm::cross(glm::vec3(plane), vec2), 1.0f);
		plane = glm::vec4(glm::normalize(glm::vec3(plane)), 1.0f);
		float D1 = -glm::dot(b, glm::vec3(plane));
		plane.w = D1;

		float &originDist1 = D1;
		float otherSideDist1 = glm::dot(glm::vec3(plane), d);
		if (otherSideDist1 < 0.001)
			otherSideDist1 = 0.0;

		if (originDist1 * otherSideDist1 <= 0.001)
		{
			triangles[0].push_back(_simplexPoints[0]);
			triangles[0].push_back(_simplexPoints[1]);
			triangles[0].push_back(_simplexPoints[2]);
			findClosestPointInSimplex(triangles[0], _outContactPoint);
			closestDistSquared = glm::length(_outContactPoint);
			closestIdx = 0;
		}

		glm::vec3 vec3 = d;
		vec3 -= b;
		plane = glm::vec4(vec2, 1.0f);
		plane = glm::vec4(glm::cross(glm::vec3(plane), vec3), 1.0f);
		plane = glm::vec4(glm::normalize(glm::vec3(plane)), 1.0f);
		float D2 = -glm::dot(b, glm::vec3(plane));
		plane.w = D2;

		float &originDist2 = D2;
		float otherSideDist2 = glm::dot(glm::vec3(plane), c);
		if (otherSideDist2 < 0.001)
			otherSideDist2 = 0.0;

		if (originDist2 * otherSideDist2 <= 0.001)
		{
			triangles[1].push_back(_simplexPoints[0]);
			triangles[1].push_back(_simplexPoints[1]);
			triangles[1].push_back(_simplexPoints[3]);
			glm::vec3 triPt;
			findClosestPointInSimplex(triangles[1], triPt);
			float distSquared = glm::length(triPt);

			if (distSquared < closestDistSquared)
			{
				_outContactPoint = triPt;
				closestDistSquared = distSquared;
				closestIdx = 1;
			}
		}

		plane = glm::vec4(vec3, 1.0);
		plane = glm::vec4(glm::cross(glm::vec3(plane), vec1), 1.0f);
		plane = glm::vec4(glm::normalize(glm::vec3(plane)), 1.0f);
		float D3 = -glm::dot(b, glm::vec3(plane));
		plane.w = D3;

		float &originDist3 = D3;
		float otherSideDist3 = glm::dot(glm::vec3(plane), a);
		if (otherSideDist3 < 0.001)
			otherSideDist3 = 0.0;

		if (originDist3 * otherSideDist3 <= 0.001)
		{
			triangles[2].push_back(_simplexPoints[2]);
			triangles[2].push_back(_simplexPoints[1]);
			triangles[2].push_back(_simplexPoints[3]);
			glm::vec3 triPt;
			findClosestPointInSimplex(triangles[2], triPt);
			float distSquared = glm::length(triPt);

			if (distSquared < closestDistSquared)
			{
				_outContactPoint = triPt;
				closestDistSquared = distSquared;
				closestIdx = 2;
			}
		}

		vec1 = a;
		vec1 -= c;
		plane = glm::vec4(d, 1.0);
		plane -= glm::vec4(c, 0.0);
		plane = glm::vec4(glm::cross(glm::vec3(plane), vec1), 1.0f);
		plane = glm::vec4(glm::normalize(glm::vec3(plane)), 1.0f);
		float D4 = -glm::dot(c, glm::vec3(plane));
		plane.w = D4;

		float &originDist4 = D4;
		float otherSideDist4 = glm::dot(glm::vec3(plane), b);
		if (otherSideDist4 < 0.001)
			otherSideDist4 = 0.0;

		if (originDist4 * otherSideDist4 <= 0.001)
		{
			triangles[3].push_back(_simplexPoints[0]);
			triangles[3].push_back(_simplexPoints[2]);
			triangles[3].push_back(_simplexPoints[3]);
			glm::vec3 triPt;
			findClosestPointInSimplex(triangles[3], triPt);
			float distSquared = glm::length(triPt);

			if (distSquared < closestDistSquared)
			{
				_outContactPoint = triPt;
				closestDistSquared = distSquared;
				closestIdx = 3;
			}
		}

		if (closestIdx >= 0)
			_simplexPoints = triangles[closestIdx];
	}
	break;
	}
}

/** Source code for the GJK algorithm can be found here:  http://vec3.ca/gjk/implementation/ */
bool checkGJKCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2, glm::vec3 &_contactN, float &_t)
{
	glm::vec3 outContactPoint;

	glm::vec3 d = glm::vec3(1.0f, 0.0f, 0.0f); // Choose a search direction
	vector<glm::vec3> simplex;

	vector<glm::vec3> vertices1, vertices2;
	getVertices(_object1, vertices1);
	getVertices(_object2, vertices2);

	glm::vec3 c = support(vertices1, vertices2, d);
	if (glm::dot(c, d) < 0)
		return false;

	simplex.push_back(c);
	d = -c;
	glm::vec3 b = support(vertices1, vertices2, d);
	if (glm::dot(b, d) < 0)
		return false;

	simplex.push_back(b);
	d = glm::cross(glm::cross(c - b, -b), c - b);

	int i;
	for (i = 0; i < 32; i++)
	{
		glm::vec3 a = support(vertices1, vertices2, d);

		// No intersection
		if (glm::dot(a, d) < 0)
			return false;

		if (update(simplex, a, d))
		{
			findClosestPointInSimplex(simplex, outContactPoint);
			_contactN = glm::normalize(outContactPoint);
			_t = glm::length(outContactPoint);
			return true;
		}
	}

	findClosestPointInSimplex(simplex, outContactPoint);
	_contactN = glm::normalize(outContactPoint);
	_t = glm::length(outContactPoint);
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

	// Apply impulses
	_object1.setVelocity(_object1.getVelocity() + deltaV1);
	_object1.setAngularVelocity(_object1.getAngularVelocity() + deltaW1);
	_object2.setVelocity(_object2.getVelocity() + deltaV2);
	_object2.setAngularVelocity(_object2.getAngularVelocity() + deltaW2);
}