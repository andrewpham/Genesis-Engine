// GL includes
#include <gengine/Model.h>
#include "LifeOfGaben.h"

genesis::InputManager _gabenGameInputManager;
genesis::ResourceManager _gabenGameResourceManager;
static GLfloat _health = 100.0f;
static GLfloat _secondsSinceDamaged = 0.0f;

Direction vectorDirection(glm::vec2);
glm::quat rotationBetweenVectors(glm::vec3, glm::vec3);
GLboolean checkCollision(genesis::GameObject3D&, genesis::InputManager&);
GLboolean checkTrapCollision(genesis::GameObject3D&, genesis::GameObject3D&);
GLboolean checkBulletCollision(genesis::Enemy&, genesis::InputManager&);
void resolveCollision(genesis::GameObject3D&, genesis::InputManager&);
void resolveEnemyInteractions(genesis::Enemy&, genesis::InputManager&, GLfloat, GLuint);
void resolveWallCollisions(GLfloat, GLfloat, GLfloat, GLfloat, genesis::InputManager&);

static inline float random_float()
{
	static unsigned int seed = 0x13371337;

	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

// Courtesy of the fine people over at StackOverflow
static inline float random_range(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void run_gaben_game(GLFWwindow* window)
{
	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup and compile our shaders
	genesis::Shader shader = _gabenGameResourceManager.loadShader("../Genesis/Shaders/Life of Gaben/object.vs", "Shaders/Life of Gaben/object.frag", "object");
	genesis::Shader skyboxShader = _gabenGameResourceManager.loadShader("../Genesis/Shaders/Life of Gaben/skybox.vs", "Shaders/Life of Gaben/skybox.frag", "skybox");
	genesis::Shader flockShader = _gabenGameResourceManager.loadShader("../Genesis/Shaders/Life of Gaben/flock.vs", "Shaders/Life of Gaben/flock.frag", "flock");
	genesis::Shader flockUpdateShader = _gabenGameResourceManager.loadShader("../Genesis/Shaders/Life of Gaben/flock.comp", "flockUpdate");

	// Flock data and state
	GLuint flockBuffers[2], flockVAO[2], geometryVBO;
	// Box data and state
	GLuint boxVAO, boxVBO;
	// Floor data and state
	GLuint floorVAO, floorVBO;
	// Wall data and state
	GLuint wallVAO, wallVBO;
	// Square data and state
	GLuint squareVAO, squareVBO;
	// Skybox data and state
	GLuint skyboxVAO, skyboxVBO;

	// Uniforms for the various shaders
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
		struct
		{
			GLuint mvp;
		} flock;
		struct
		{
			GLint goal;
		} flockUpdate;
	} uniforms;

	// Cache the uniform locations
	shader.Use();
	uniforms.object.view = glGetUniformLocation(shader.ID, "view");
	uniforms.object.projection = glGetUniformLocation(shader.ID, "projection");
	uniforms.object.lightPos = glGetUniformLocation(shader.ID, "lightPos");
	uniforms.object.viewPos = glGetUniformLocation(shader.ID, "viewPos");
	skyboxShader.Use();
	uniforms.skybox.view = glGetUniformLocation(skyboxShader.ID, "view");
	uniforms.skybox.projection = glGetUniformLocation(skyboxShader.ID, "projection");
	flockShader.Use();
	uniforms.flock.mvp = glGetUniformLocation(flockShader.ID, "mvp");
	flockUpdateShader.Use();
	uniforms.flockUpdate.goal = glGetUniformLocation(flockUpdateShader.ID, "goal");

	// Index to swap flock buffers with one another to be used in the rendering and computation/data upload stages and vice versa
	GLuint frameIndex = 0;
	// Used to pace the spawning of objects at regular intervals
	GLfloat secondsSincePickup = 0.0f;
	GLfloat secondsSinceEnemy = 0.0f;
	GLfloat secondsSinceTrap = 0.0f;
	GLfloat attackCooldown = 0.0f;
	GLint numTrapsAvailable = 0;

#pragma region "object_initialization"
	/** Setup box VAO */
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BOX_VERTICES), &BOX_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);
	/** Setup floor VAO */
	// Floor quad positions
	glm::vec3 floorPos1(-30.0f, 30.0f, 0.0f);
	glm::vec3 floorPos2(-30.0f, -30.0f, 0.0f);
	glm::vec3 floorPos3(30.0f, -30.0f, 0.0f);
	glm::vec3 floorPos4(30.0f, 30.0f, 0.0f);
	// Quad texture coordinates
	glm::vec2 floorUV1(0.0f, 30.0f);
	glm::vec2 floorUV2(0.0f, 0.0f);
	glm::vec2 floorUV3(30.0f, 0.0f);
	glm::vec2 floorUV4(30.0f, 30.0f);
	// Quad normal vector
	glm::vec3 nm(0.0, 0.0, 1.0);
	// Calculate tangent/bitangent vectors of both triangles
	glm::vec3 floorTangent1, floorBitangent1;
	glm::vec3 floorTangent2, floorBitangent2;
	genesis::computeTangentBasis(floorPos1, floorPos2, floorPos3, floorPos4,
		floorUV1, floorUV2, floorUV3, floorUV4,
		nm,
		floorTangent1, floorBitangent1,
		floorTangent2, floorBitangent2);
	GLfloat floorVertices[] = {
		// Positions						   // Normal         // TexCoords			 // Tangent											// Bitangent
		floorPos1.x, floorPos1.y, floorPos1.z, nm.x, nm.y, nm.z, floorUV1.x, floorUV1.y, floorTangent1.x, floorTangent1.y, floorTangent1.z, floorBitangent1.x, floorBitangent1.y, floorBitangent1.z,
		floorPos2.x, floorPos2.y, floorPos2.z, nm.x, nm.y, nm.z, floorUV2.x, floorUV2.y, floorTangent1.x, floorTangent1.y, floorTangent1.z, floorBitangent1.x, floorBitangent1.y, floorBitangent1.z,
		floorPos3.x, floorPos3.y, floorPos3.z, nm.x, nm.y, nm.z, floorUV3.x, floorUV3.y, floorTangent1.x, floorTangent1.y, floorTangent1.z, floorBitangent1.x, floorBitangent1.y, floorBitangent1.z,

		floorPos1.x, floorPos1.y, floorPos1.z, nm.x, nm.y, nm.z, floorUV1.x, floorUV1.y, floorTangent2.x, floorTangent2.y, floorTangent2.z, floorBitangent2.x, floorBitangent2.y, floorBitangent2.z,
		floorPos3.x, floorPos3.y, floorPos3.z, nm.x, nm.y, nm.z, floorUV3.x, floorUV3.y, floorTangent2.x, floorTangent2.y, floorTangent2.z, floorBitangent2.x, floorBitangent2.y, floorBitangent2.z,
		floorPos4.x, floorPos4.y, floorPos4.z, nm.x, nm.y, nm.z, floorUV4.x, floorUV4.y, floorTangent2.x, floorTangent2.y, floorTangent2.z, floorBitangent2.x, floorBitangent2.y, floorBitangent2.z
	};
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	glBindVertexArray(0);
	/** Setup wall VAO */
	// Wall quad positions
	glm::vec3 wallPos1(-40.0f, 1.0f, 0.0f);
	glm::vec3 wallPos2(-40.0f, -1.0f, 0.0f);
	glm::vec3 wallPos3(40.0f, -1.0f, 0.0f);
	glm::vec3 wallPos4(40.0f, 1.0f, 0.0f);
	// Wall texture coordinates
	glm::vec2 wallUV1(0.0f, 1.0f);
	glm::vec2 wallUV2(0.0f, 0.0f);
	glm::vec2 wallUV3(40.0f, 0.0f);
	glm::vec2 wallUV4(40.0f, 1.0f);
	// Calculate tangent/bitangent vectors of both triangles
	glm::vec3 wallTangent1, wallBitangent1;
	glm::vec3 wallTangent2, wallBitangent2;
	genesis::computeTangentBasis(wallPos1, wallPos2, wallPos3, wallPos4,
		wallUV1, wallUV2, wallUV3, wallUV4,
		nm,
		wallTangent1, wallBitangent1,
		wallTangent2, wallBitangent2);
	GLfloat wallVertices[] = {
		// Positions						// Normal         // TexCoords			// Tangent										// Bitangent
		wallPos1.x, wallPos1.y, wallPos1.z, nm.x, nm.y, nm.z, wallUV1.x, wallUV1.y, wallTangent1.x, wallTangent1.y, wallTangent1.z, wallBitangent1.x, wallBitangent1.y, wallBitangent1.z,
		wallPos2.x, wallPos2.y, wallPos2.z, nm.x, nm.y, nm.z, wallUV2.x, wallUV2.y, wallTangent1.x, wallTangent1.y, wallTangent1.z, wallBitangent1.x, wallBitangent1.y, wallBitangent1.z,
		wallPos3.x, wallPos3.y, wallPos3.z, nm.x, nm.y, nm.z, wallUV3.x, wallUV3.y, wallTangent1.x, wallTangent1.y, wallTangent1.z, wallBitangent1.x, wallBitangent1.y, wallBitangent1.z,

		wallPos1.x, wallPos1.y, wallPos1.z, nm.x, nm.y, nm.z, wallUV1.x, wallUV1.y, wallTangent2.x, wallTangent2.y, wallTangent2.z, wallBitangent2.x, wallBitangent2.y, wallBitangent2.z,
		wallPos3.x, wallPos3.y, wallPos3.z, nm.x, nm.y, nm.z, wallUV3.x, wallUV3.y, wallTangent2.x, wallTangent2.y, wallTangent2.z, wallBitangent2.x, wallBitangent2.y, wallBitangent2.z,
		wallPos4.x, wallPos4.y, wallPos4.z, nm.x, nm.y, nm.z, wallUV4.x, wallUV4.y, wallTangent2.x, wallTangent2.y, wallTangent2.z, wallBitangent2.x, wallBitangent2.y, wallBitangent2.z
	};
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), &wallVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	glBindVertexArray(0);
	/** Setup square VAO */
	// Square quad positions
	glm::vec3 squarePos1(-0.35f, 0.35f, 0.0f);
	glm::vec3 squarePos2(-0.35f, -0.35f, 0.0f);
	glm::vec3 squarePos3(0.35f, -0.35f, 0.0f);
	glm::vec3 squarePos4(0.35f, 0.35f, 0.0f);
	// Square texture coordinates
	glm::vec2 squareUV1(0.0f, 1.0f);
	glm::vec2 squareUV2(0.0f, 0.0f);
	glm::vec2 squareUV3(1.0f, 0.0f);
	glm::vec2 squareUV4(1.0f, 1.0f);
	// Calculate tangent/bitangent vectors of both triangles
	glm::vec3 squareTangent1, squareBitangent1;
	glm::vec3 squareTangent2, squareBitangent2;
	genesis::computeTangentBasis(squarePos1, squarePos2, squarePos3, squarePos4,
		squareUV1, squareUV2, squareUV3, squareUV4,
		nm,
		squareTangent1, squareBitangent1,
		squareTangent2, squareBitangent2);
	GLfloat squareVertices[] = {
		// Positions						// Normal         // TexCoords			// Tangent										// Bitangent
		squarePos1.x, squarePos1.y, squarePos1.z, nm.x, nm.y, nm.z, squareUV1.x, squareUV1.y, squareTangent1.x, squareTangent1.y, squareTangent1.z, squareBitangent1.x, squareBitangent1.y, squareBitangent1.z,
		squarePos2.x, squarePos2.y, squarePos2.z, nm.x, nm.y, nm.z, squareUV2.x, squareUV2.y, squareTangent1.x, squareTangent1.y, squareTangent1.z, squareBitangent1.x, squareBitangent1.y, squareBitangent1.z,
		squarePos3.x, squarePos3.y, squarePos3.z, nm.x, nm.y, nm.z, squareUV3.x, squareUV3.y, squareTangent1.x, squareTangent1.y, squareTangent1.z, squareBitangent1.x, squareBitangent1.y, squareBitangent1.z,

		squarePos1.x, squarePos1.y, squarePos1.z, nm.x, nm.y, nm.z, squareUV1.x, squareUV1.y, squareTangent2.x, squareTangent2.y, squareTangent2.z, squareBitangent2.x, squareBitangent2.y, squareBitangent2.z,
		squarePos3.x, squarePos3.y, squarePos3.z, nm.x, nm.y, nm.z, squareUV3.x, squareUV3.y, squareTangent2.x, squareTangent2.y, squareTangent2.z, squareBitangent2.x, squareBitangent2.y, squareBitangent2.z,
		squarePos4.x, squarePos4.y, squarePos4.z, nm.x, nm.y, nm.z, squareUV4.x, squareUV4.y, squareTangent2.x, squareTangent2.y, squareTangent2.z, squareBitangent2.x, squareBitangent2.y, squareBitangent2.z
	};
	glGenVertexArrays(1, &squareVAO);
	glGenBuffers(1, &squareVBO);
	glBindVertexArray(squareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), &squareVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	glBindVertexArray(0);
	/** Setup skybox VAO */
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), &SKYBOX_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
#pragma endregion

#pragma region "flock_initialization"
	// Allocate space for shader storage buffer objects to store and retrieve data pertaining to members of the flock; also bind those objects to the current context
	glGenBuffers(2, flockBuffers);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flockBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(Fish), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flockBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(Fish), NULL, GL_DYNAMIC_COPY);

	// Upload flock member vertex data to be used during the rendering stage
	glGenBuffers(1, &geometryVBO);
	glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FISH_VERTICES), FISH_VERTICES, GL_STATIC_DRAW);

	// Setup flock VAO
	glGenVertexArrays(2, flockVAO);

	int i;

	for (i = 0; i < 2; i++)
	{
		// Bind the geometry data of a flock member to the shaders
		glBindVertexArray(flockVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)(6 * sizeof(glm::vec3)));
		// Bind the storage into which the shaders can write computed data
		glBindBuffer(GL_ARRAY_BUFFER, flockBuffers[i]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Fish), NULL);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Fish), (void *)sizeof(glm::vec4));
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	// Map the entirety of a buffer object's data store into the client's address space so we can write data to it
	glBindBuffer(GL_ARRAY_BUFFER, flockBuffers[0]);
	Fish * ptr = reinterpret_cast<Fish *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, FLOCK_SIZE * sizeof(Fish), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (i = 0; i < FLOCK_SIZE; i++)
	{
		ptr[i].position = (glm::vec3(random_float(), random_float(), random_float()) - glm::vec3(0.5f)) * 300.0f;
		ptr[i].velocity = (glm::vec3(random_float(), random_float(), random_float()) - glm::vec3(0.5f));
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
#pragma endregion

	// Load textures
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/Life of Gaben/ground.jpg", false, "floor");
	GLuint floorTexture = _gabenGameResourceManager.getTexture("floor").ID;
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/Life of Gaben/ground_normal.jpg", false, "floorNormalMap");
	GLuint floorNormalMap = _gabenGameResourceManager.getTexture("floorNormalMap").ID;
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/Life of Gaben/brickwall.jpg", false, "wall");
	GLuint wallTexture = _gabenGameResourceManager.getTexture("wall").ID;
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/Life of Gaben/brickwall_normal.jpg", false, "wallNormalMap");
	GLuint wallNormalMap = _gabenGameResourceManager.getTexture("wallNormalMap").ID;
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/container.jpg", false, "tower");
	GLuint towerTexture = _gabenGameResourceManager.getTexture("tower").ID;
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/Life of Gaben/tower_head.jpg", false, "towerHead");
	GLuint towerHeadTexture = _gabenGameResourceManager.getTexture("towerHead").ID;
	_gabenGameResourceManager.loadTexture("../Genesis/Textures/Life of Gaben/square.png", false, "square");
	GLuint squareTexture = _gabenGameResourceManager.getTexture("square").ID;

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("../Genesis/Textures/Skybox/Life of Gaben/right.jpg");
	faces.push_back("../Genesis/Textures/Skybox/Life of Gaben/left.jpg");
	faces.push_back("../Genesis/Textures/Skybox/Life of Gaben/top.jpg");
	faces.push_back("../Genesis/Textures/Skybox/Life of Gaben/bottom.jpg");
	faces.push_back("../Genesis/Textures/Skybox/Life of Gaben/back.jpg");
	faces.push_back("../Genesis/Textures/Skybox/Life of Gaben/front.jpg");
	_gabenGameResourceManager.loadCubemap(faces);
	GLuint cubemapTexture = _gabenGameResourceManager.getCubemap();

	// Load models
	genesis::Model house("../Genesis/Objects/Life of Gaben/House/Farmhouse OBJ.obj");
	genesis::Model rock("../Genesis/Objects/Rock/rock.obj");
	genesis::Model pickup("../Genesis/Objects/Life of Gaben/Pickup/cup OBJ.obj");
	genesis::Model enemy("../Genesis/Objects/Nanosuit/nanosuit.obj");
	genesis::Model crosshair("../Genesis/Objects/Life of Gaben/Crosshair/sphere.obj");
	genesis::Model gun("../Genesis/Objects/Life of Gaben/Gun/M4A1.obj");

	// Create game objects
	genesis::GameObject3D floorObject(shader, floorTexture, floorVAO, 6, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	floorObject.setNormalMap(floorNormalMap);
	floorObject.setHasNormalMap(true);
	genesis::GameObject3D houseObject(shader, house, glm::vec3(-30.0f, -1.05f, 5.0f), glm::vec3(0.55f, 0.55f, 0.55f));
	vector<genesis::Enemy> enemyObjects;
	vector<genesis::GameObject3D> pickupObjects;
	// Little hack to ensure that the wall textures actually render - not sure why this works!
	pickupObjects.push_back(genesis::GameObject3D(shader, pickup, glm::vec3(20.0f, 0.0f, 30.0f), glm::vec3(0.025f, 0.025f, 0.025f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
	vector<genesis::GameObject3D> wallObjects;
	GLfloat west = -12.f, east = 14.f, south = 25.f, north = -10.f;
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(east, 0.0f, 0.0f), glm::vec3(1.0f), 90.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(west, 0.0f, 0.0f), glm::vec3(1.0f), 270.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(0.0f, 0.0f, north), glm::vec3(1.0f), 180.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	wallObjects.push_back(genesis::GameObject3D(shader, wallTexture, wallVAO, 6, glm::vec3(0.0f, 0.0f, south)));
	for (genesis::GameObject3D &wallObject : wallObjects)
	{
		wallObject.setNormalMap(wallNormalMap);
		wallObject.setHasNormalMap(true);
	}
	vector<genesis::GameObject3D> rockObjects;
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(0.0f, -0.95f, 19.0f), glm::vec3(0.25f, 0.25f, 0.25f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(0.0f, -0.95f, 8.0f), glm::vec3(0.25f, 0.25f, 0.25f), 45.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(-4.5f, -0.95f, 13.0f), glm::vec3(0.25f, 0.25f, 0.25f), 90.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(5.0f, -0.95f, 13.0f), glm::vec3(0.25f, 0.25f, 0.25f), 135.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(-3.0f, -0.95f, 10.0f), glm::vec3(0.25f, 0.25f, 0.25f), 180.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(3.5f, -0.95f, 10.0f), glm::vec3(0.25f, 0.25f, 0.25f), 235.f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(-3.0f, -0.95f, 17.0f), glm::vec3(0.25f, 0.25f, 0.25f), 22.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
	rockObjects.push_back(genesis::GameObject3D(shader, rock, glm::vec3(3.5f, -0.95f, 17.0f), glm::vec3(0.25f, 0.25f, 0.25f), 67.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
	for (genesis::GameObject3D &rockObject : rockObjects)
	{
		rockObject.setHitboxRadius(0.46f);
		rockObject.setHitboxOffset(glm::vec3(0.0f, -0.1f, 0.0f));
	}
	vector<genesis::GameObject3D> boxObjects;
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(-11.0f, 0.0f, -9.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(-11.0f, 0.0f, 24.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(13.0f, 0.0f, 24.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(13.0f, 0.0f, -9.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(6.0f, 0.0f, -3.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(-5.0f, 0.0f, -2.0f)));
	boxObjects.push_back(genesis::GameObject3D(shader, wallTexture, boxVAO, 36, glm::vec3(0.0f, 0.0f, -7.0f)));
	for (genesis::GameObject3D &boxObject : boxObjects)
	{
		boxObject.setHitboxRadius(1.5f);
		boxObject.setHitboxOffset(glm::vec3(0.0f));
	}
	vector<genesis::GameObject3D> towerObjects;
	vector<genesis::GameObject3D> towerHeadObjects;
	genesis::GameObject3D squareObject(shader, squareTexture, squareVAO, 6, glm::vec3(0.0f, -0.99f, 0.0f), glm::vec3(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	genesis::GameObject3D crossHairObject(shader, crosshair, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f));
	genesis::GameObject3D gunObject(shader, gun, glm::vec3(0.2f, -0.2f, -0.5f), glm::vec3(0.03f));

	glEnable(GL_DEPTH_TEST);

	// Set the light source properties in the fragment shader
	shader.Use();
	glUniform3f(uniforms.object.lightPos, LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z);

	// Play theme song
	_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/theme.mp3", GL_TRUE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		_gabenGameInputManager.setDeltaTime(currentFrame - _gabenGameInputManager.getLastFrame());
		_gabenGameInputManager.setLastFrame(currentFrame);

		// Check and call events
		glfwPollEvents();
		_gabenGameInputManager.checkKeysPressed();

		// Clear buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Slowly decays the movement speed of Gaben if greater than base speed
		if (_gabenGameInputManager._camera.MovementSpeed > 3.0f)
			_gabenGameInputManager._camera.MovementSpeed -= _gabenGameInputManager.getDeltaTime() / 7.0f;
		// Sets the maximum movement speed of the player
		if (_gabenGameInputManager._camera.MovementSpeed > 12.0f)
			_gabenGameInputManager._camera.MovementSpeed = 12.0f;
		// Controls the gunshot cooldown and later resets the cooldown timer once it expires
		if (_gabenGameInputManager._keys[GLFW_KEY_SPACE])
			attackCooldown += _gabenGameInputManager.getDeltaTime();
		// Plays the gunshot sound every time we fire a bullet
		if (attackCooldown > 0.075f)
			_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/gunshot.mp3", GL_FALSE);

		// Set the view position property in the fragment shader
		shader.Use();
		glUniform3f(uniforms.object.viewPos, _gabenGameInputManager._camera.Position.x, _gabenGameInputManager._camera.Position.y, _gabenGameInputManager._camera.Position.z);

		// Draw skybox first
		glDepthMask(GL_FALSE); // Remember to turn depth writing off

		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(_gabenGameInputManager._camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(_gabenGameInputManager._camera.Zoom, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(uniforms.skybox.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniforms.skybox.projection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE); // Turn depth writing back on

		// Then draw scene as normal
		shader.Use();
		view = _gabenGameInputManager._camera.GetViewMatrix();
		glUniformMatrix4fv(uniforms.object.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniforms.object.projection, 1, GL_FALSE, glm::value_ptr(projection));

		// Floor
		floorObject.render();
		// House
		houseObject.render();
		// Enemy Spawns
		secondsSinceEnemy += _gabenGameInputManager.getDeltaTime();
		if (secondsSinceEnemy >= 20.0f)
		{
			secondsSinceEnemy = 0.0f;
			GLfloat x_rand = random_range(west + 2, east - 2);
			GLfloat z_rand = random_range(0.0f, south - 2);
			enemyObjects.push_back(genesis::Enemy(shader, enemy, glm::vec3(x_rand, -0.2f, z_rand), glm::vec3(0.10f, 0.10f, 0.10f)));
			enemyObjects.back().setHitboxRadius(0.4f);
			enemyObjects.back().setHitboxOffset(glm::vec3(0.0f, 0.8f, 0.0f));
			enemyObjects.back().setAggroRadius(5.0f);
			enemyObjects.back().setDamageRadius(2.0f);
		}
		for (genesis::Enemy &enemyObject : enemyObjects)
		{
			if (!enemyObject.getDestroyed())
			{
				enemyObject.setPositionY(-0.2f + sinf(currentFrame) / 4);
				enemyObject.render();
				resolveCollision(enemyObject, _gabenGameInputManager);
				resolveEnemyInteractions(enemyObject, _gabenGameInputManager, _gabenGameInputManager.getDeltaTime(), DAMAGE);
				// Checks any bullet collisions with the enemy actor if the F key is pressed
				if (_gabenGameInputManager._keys[GLFW_KEY_SPACE] && attackCooldown > 0.075f && checkBulletCollision(enemyObject, _gabenGameInputManager))
				{
					enemyObject.setHealth(enemyObject.getHealth() - 1);
					if (enemyObject.getHealth() < 1)
					{
						enemyObject.setDestroyed(true);
						_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/death.wav", GL_FALSE);
					}
				}
			}
		}
		// Print the health of the player
		//std::cout << _health << std::endl;
		// Pickup Spawns
		secondsSincePickup += _gabenGameInputManager.getDeltaTime();
		if (secondsSincePickup >= 20.0f)
		{
			secondsSincePickup = 0.0f;
			GLfloat x_rand = random_range(west + 2, east - 2);
			GLfloat z_rand = random_range(0.0f, south - 2);
			GLfloat theta_rand = random_range(0.0f, 360.0f);
			pickupObjects.push_back(genesis::GameObject3D(shader, pickup, glm::vec3(x_rand, -0.75f, z_rand), glm::vec3(0.025f, 0.025f, 0.025f), theta_rand, glm::vec3(0.0f, 1.0f, 0.0f)));
			pickupObjects.back().setHitboxRadius(0.22f);
			pickupObjects.back().setHitboxOffset(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		for (genesis::GameObject3D &pickupObject : pickupObjects)
		{
			if (!pickupObject.getDestroyed())
			{
				if (pickupObject._rotationAngle > 360.f)
					pickupObject._rotationAngle = 0.0f;
				pickupObject._rotationAngle = pickupObject._rotationAngle + _gabenGameInputManager.getDeltaTime();
				pickupObject.render();
			}
			if (!pickupObject.getDestroyed() && checkCollision(pickupObject, _gabenGameInputManager))
			{
				pickupObject.setDestroyed(true);
				_gabenGameInputManager._camera.MovementSpeed *= 2.0f;
				_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/stim.wav", GL_FALSE);
			}
		}
		// Walls
		for (genesis::GameObject3D &wallObject : wallObjects)
		{
			wallObject.render();
		}
		resolveWallCollisions(north, south, east, west, _gabenGameInputManager);
		// Rocks
		for (genesis::GameObject3D &rockObject : rockObjects)
		{
			rockObject.render();
			resolveCollision(rockObject, _gabenGameInputManager);
		}
		// Boxes
		for (genesis::GameObject3D &boxObject : boxObjects)
		{
			boxObject.render();
			resolveCollision(boxObject, _gabenGameInputManager);
		}
		// Towers
		// Calculates the world position of where the player is looking at at most one unit away from the player
		GLfloat x_ray = _gabenGameInputManager._camera.Position.x + 3 * _gabenGameInputManager._camera.Front.x;
		GLfloat y_ray = _gabenGameInputManager._camera.Position.y + 3 * _gabenGameInputManager._camera.Front.y;
		GLfloat z_ray = _gabenGameInputManager._camera.Position.z + 3 * _gabenGameInputManager._camera.Front.z;
		// Keeps track of when we can spawn a new tower defense (once every 15 seconds by default)
		secondsSinceTrap += _gabenGameInputManager.getDeltaTime();
		if (secondsSinceTrap > TOWER_SPAWN_RATE)
		{
			secondsSinceTrap = 0.0f;
			numTrapsAvailable += 1;
		}
		if (numTrapsAvailable > 0 && _gabenGameInputManager._keys[GLFW_KEY_E])
		{	
			numTrapsAvailable -= 1;

			towerObjects.push_back(genesis::GameObject3D(shader, towerTexture, boxVAO, 36, glm::vec3(x_ray, 0.0f, z_ray), glm::vec3(0.1f, 1.0f, 0.1f)));			
			towerObjects.back().setHitboxRadius(0.35f);
			towerObjects.back().setHitboxOffset(glm::vec3(0.0f, 0.0f, 0.0f));
			towerHeadObjects.push_back(genesis::GameObject3D(shader, towerHeadTexture, boxVAO, 36, glm::vec3(x_ray, 0.8f, z_ray), glm::vec3(0.2f, 0.2f, 0.2f)));
			_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/towerplc.wav", GL_FALSE);
		}
		for (int i = 0; i < towerObjects.size(); i++)
		{
			if (!towerObjects[i].getDestroyed())
			{
				if (towerObjects[i]._rotationAngle > 360.f)
					towerObjects[i]._rotationAngle = 0.0f;
				towerObjects[i]._rotationAngle = towerObjects[i]._rotationAngle + _gabenGameInputManager.getDeltaTime();
				towerObjects[i].render();
				resolveCollision(towerObjects[i], _gabenGameInputManager);
				if (towerHeadObjects[i]._rotationAngle < -360.f)
					towerHeadObjects[i]._rotationAngle = 0.0f;
				towerHeadObjects[i]._rotationAngle = towerHeadObjects[i]._rotationAngle - _gabenGameInputManager.getDeltaTime();
				towerHeadObjects[i].render();
				resolveCollision(towerHeadObjects[i], _gabenGameInputManager);
			}
			for (genesis::Enemy &enemyObject : enemyObjects)
			{
				if (!towerObjects[i].getDestroyed() && checkTrapCollision(towerObjects[i], enemyObject))
				{
					towerObjects[i].setDestroyed(true);
					towerHeadObjects[i].setDestroyed(true);
					enemyObject.setDestroyed(true);
					_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/explosion.wav", GL_FALSE);
					_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/death.wav", GL_FALSE);
				}
			}
		}
		// Print the number of traps available to place
		std::cout << numTrapsAvailable << std::endl;
		// Square
		squareObject.setPosition(glm::vec3(x_ray, -0.99f, z_ray));
		squareObject.render();
		// Crosshair
		crossHairObject.setPosition(glm::vec3(x_ray, y_ray, z_ray));
		crossHairObject.render();
		// Gun
		view = glm::mat4();
		glUniformMatrix4fv(uniforms.object.view, 1, GL_FALSE, glm::value_ptr(view));
		gunObject.render();
		// Resets the cooldown tracker once the variable for the current attack cooldown is used in this frame
		if (attackCooldown > 0.075f)
			attackCooldown = 0.0f;
		view = _gabenGameInputManager._camera.GetViewMatrix();
#pragma region "flock_render"
		flockUpdateShader.Use();
		// Shift the flock convergence point over time to create a more dynamic simulation
		glm::vec3 goal = glm::vec3(sinf(currentFrame * 0.34f * PI_F / 180.f),
			cosf(currentFrame * 0.29f * PI_F / 180.f),
			sinf(currentFrame * 0.12f * PI_F / 180.f) * cosf(currentFrame * 0.5f * PI_F / 180.f));
		goal = goal * glm::vec3(35.0f, 105.0f, 60.0f);
		glUniform3fv(uniforms.flockUpdate.goal, 1, glm::value_ptr(goal));

		// Swap the flock buffers (one will be used for rendering and the other will be written 
		// into from the shader for the next frame, when it will be used for rendering)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flockBuffers[frameIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flockBuffers[frameIndex ^ 1]);

		glDispatchCompute(NUM_WORKGROUPS, 1, 1);

		flockShader.Use();
		glm::mat4 mvp = projection * view;
		glUniformMatrix4fv(uniforms.flock.mvp, 1, GL_FALSE, glm::value_ptr(mvp));

		// Render the flock
		glBindVertexArray(flockVAO[frameIndex]);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, FLOCK_SIZE);
		frameIndex ^= 1;
#pragma endregion

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

// Calculates which direction a vector is facing (N,E,S or W)
Direction vectorDirection(glm::vec2 _target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(_target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

/** http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/ */
glm::quat rotationBetweenVectors(glm::vec3 _start, glm::vec3 _dest) 
{
	_start = glm::normalize(_start);
	_dest = glm::normalize(_dest);

	float cosTheta = dot(_start, _dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) 
	{
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to _start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), _start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), _start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = cross(_start, _dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(s * 0.5f, rotationAxis.x * invs, 
		rotationAxis.y * invs, rotationAxis.z * invs);
}

GLboolean checkCollision(genesis::GameObject3D &_object, genesis::InputManager &_inputManager)
{
	glm::vec3 hitboxPosition = _object.getPosition() + _object.getHitboxOffset();
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	/** Collision detection booleans */
	// Collision right of the hitbox?
	bool collisionX = hitboxPosition.x + _object.getHitboxRadius() >= cameraPosition.x &&
		cameraPosition.x >= hitboxPosition.x;
	// Collision behind the hitbox?
	bool collisionZ = hitboxPosition.z + _object.getHitboxRadius() >= cameraPosition.z &&
		cameraPosition.z >= hitboxPosition.z;
	// Collision left of the hitbox?
	bool collisionX2 = hitboxPosition.x - _object.getHitboxRadius() <= cameraPosition.x &&
		cameraPosition.x <= hitboxPosition.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = hitboxPosition.z - _object.getHitboxRadius() <= cameraPosition.z &&
		cameraPosition.z <= hitboxPosition.z;

	return (collisionX && collisionZ) || (collisionX2 && collisionZ)
		|| (collisionX && collisionZ2) || (collisionX2 && collisionZ2);
}

GLboolean checkTrapCollision(genesis::GameObject3D &_object1, genesis::GameObject3D &_object2)
{
	glm::vec3 hitboxPosition1 = _object1.getPosition() + _object1.getHitboxOffset();
	glm::vec3 hitboxPosition2 = _object2.getPosition() + _object2.getHitboxOffset();
	GLfloat object1XMax = hitboxPosition1.x + _object1.getHitboxRadius() + 0.15f;
	GLfloat object1XMin = hitboxPosition1.x - _object1.getHitboxRadius() - 0.15f;
	GLfloat object1ZMax = hitboxPosition1.z + _object1.getHitboxRadius() + 0.15f;
	GLfloat object1ZMin = hitboxPosition1.z - _object1.getHitboxRadius() - 0.15f;
	GLfloat object2XMax = hitboxPosition2.x + _object2.getHitboxRadius();
	GLfloat object2XMin = hitboxPosition2.x - _object2.getHitboxRadius();
	GLfloat object2ZMax = hitboxPosition2.z + _object2.getHitboxRadius();
	GLfloat object2ZMin = hitboxPosition2.z - _object2.getHitboxRadius();

	/** Collision detection booleans */
	// Collision right of the hitbox?
	bool collisionX = object1XMax >= object2XMin &&
		object2XMin >= hitboxPosition1.x;
	// Collision behind the hitbox?
	bool collisionZ = object1ZMax >= object2ZMin &&
		object2ZMin >= hitboxPosition1.z;
	// Collision left of the hitbox?
	bool collisionX2 = object1XMin <= object2XMax &&
		object2XMax <= hitboxPosition1.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = object1ZMin <= object2ZMax &&
		object2ZMax <= hitboxPosition1.z;

	return (collisionX && collisionZ) || (collisionX2 && collisionZ)
		|| (collisionX && collisionZ2) || (collisionX2 && collisionZ2);
}

/** http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms */
GLboolean checkBulletCollision(genesis::Enemy &_enemy, genesis::InputManager &_inputManager)
{
	// dir is the unit direction vector of the ray
	glm::vec3 dir = _inputManager._camera.Front;
	glm::vec3 dirfrac;
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;
	// lb is the corner of the AABB with minimal coordinates, rt is the maximal corner
	glm::vec3 enemyPos = _enemy.getPosition();
	GLfloat hitboxRadius = _enemy.getHitboxRadius();
	glm::vec3 hitboxOffset = _enemy.getHitboxOffset();
	glm::vec3 lb = glm::vec3(enemyPos.x + hitboxOffset.x - hitboxRadius, enemyPos.y + hitboxOffset.y - (hitboxRadius + 0.4f), enemyPos.z + hitboxOffset.z - hitboxRadius);
	glm::vec3 rt = glm::vec3(enemyPos.x + hitboxOffset.x + hitboxRadius, enemyPos.y + hitboxOffset.y + (hitboxRadius + 0.4f), enemyPos.z + hitboxOffset.z + hitboxRadius);
	// org is the origin of the ray
	glm::vec3 org = _inputManager._camera.Position;

	float t1 = (lb.x - org.x) * dirfrac.x;
	float t2 = (rt.x - org.x) * dirfrac.x;
	float t3 = (lb.y - org.y) * dirfrac.y;
	float t4 = (rt.y - org.y) * dirfrac.y;
	float t5 = (lb.z - org.z) * dirfrac.z;
	float t6 = (rt.z - org.z) * dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		return false;
	}

	return true;
}

void resolveCollision(genesis::GameObject3D &_object, genesis::InputManager &_inputManager)
{
	glm::vec3 hitboxPosition = _object.getPosition() + _object.getHitboxOffset();
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	/** Collision detection booleans */
	// Collision right of the hitbox?
	bool collisionX = hitboxPosition.x + _object.getHitboxRadius() >= cameraPosition.x &&
		cameraPosition.x >= hitboxPosition.x;
	// Collision behind the hitbox?
	bool collisionZ = hitboxPosition.z + _object.getHitboxRadius() >= cameraPosition.z &&
		cameraPosition.z >= hitboxPosition.z;
	// Collision left of the hitbox?
	bool collisionX2 = hitboxPosition.x - _object.getHitboxRadius() <= cameraPosition.x &&
		cameraPosition.x <= hitboxPosition.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = hitboxPosition.z - _object.getHitboxRadius() <= cameraPosition.z &&
		cameraPosition.z <= hitboxPosition.z;

	Direction dir = vectorDirection(glm::vec2(cameraPosition.x - hitboxPosition.x, cameraPosition.z - hitboxPosition.z));

	if (collisionX && collisionZ)
	{
		GLfloat penetrationX = hitboxPosition.x + _object.getHitboxRadius() - cameraPosition.x;
		GLfloat penetrationZ = hitboxPosition.z + _object.getHitboxRadius() - cameraPosition.z;
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x += penetrationX;
		else
			_inputManager._camera.Position.z += penetrationZ;
	}
	else if (collisionX2 && collisionZ)
	{
		GLfloat penetrationX = cameraPosition.x - (hitboxPosition.x - _object.getHitboxRadius());
		GLfloat penetrationZ = hitboxPosition.z + _object.getHitboxRadius() - cameraPosition.z;
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x -= penetrationX;
		else
			_inputManager._camera.Position.z += penetrationZ;
	}
	else if (collisionX && collisionZ2)
	{
		GLfloat penetrationX = hitboxPosition.x + _object.getHitboxRadius() - cameraPosition.x;
		GLfloat penetrationZ = cameraPosition.z - (hitboxPosition.z - _object.getHitboxRadius());
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x += penetrationX;
		else
			_inputManager._camera.Position.z -= penetrationZ;
	}
	else if (collisionX2 && collisionZ2)
	{
		GLfloat penetrationX = cameraPosition.x - (hitboxPosition.x - _object.getHitboxRadius());
		GLfloat penetrationZ = cameraPosition.z - (hitboxPosition.z - _object.getHitboxRadius());
		if (dir == LEFT || dir == RIGHT)
			_inputManager._camera.Position.x -= penetrationX;
		else
			_inputManager._camera.Position.z -= penetrationZ;
	}
}

void resolveEnemyInteractions(genesis::Enemy &_object, genesis::InputManager &_inputManager, GLfloat _velocity, GLuint _damage)
{
	glm::vec3 hitboxPosition = _object.getPosition() + _object.getHitboxOffset();
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	if (_object.getIsAggroed())
	{
		// Make the enemy move towards the player
		glm::vec3 dir = _inputManager._camera.Position - _object.getPosition();
		dir = glm::normalize(dir);
		_object.setPosition(_object.getPosition() + 3 * _velocity * dir);
		// Make the enemy rotate towards the player
		dir = glm::vec3(dir.x, 0.0f, dir.z);
		dir = glm::normalize(dir);
		// Find the rotation between the front of the object (that we assume towards +Z,
		// but this depends on your model) and the desired direction
		glm::quat targetOrientation = rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), dir);
		// Interpolate between start orientation and target orientation
		_object.setOrientation(glm::slerp(_object.getOrientation(), targetOrientation, _velocity));
		_object._rotationAngle = 2 * acosf(_object.getOrientation().w);

		/** Collision detection booleans */
		// Collision right of the hitbox?
		bool collisionX = hitboxPosition.x + _object.getDamageRadius() >= cameraPosition.x &&
			cameraPosition.x >= hitboxPosition.x;
		// Collision behind the hitbox?
		bool collisionZ = hitboxPosition.z + _object.getDamageRadius() >= cameraPosition.z &&
			cameraPosition.z >= hitboxPosition.z;
		// Collision left of the hitbox?
		bool collisionX2 = hitboxPosition.x - _object.getDamageRadius() <= cameraPosition.x &&
			cameraPosition.x <= hitboxPosition.x;
		// Collision in front of the hitbox?
		bool collisionZ2 = hitboxPosition.z - _object.getDamageRadius() <= cameraPosition.z &&
			cameraPosition.z <= hitboxPosition.z;

		if (_health > 0 && ((collisionX && collisionZ) || (collisionX2 && collisionZ) ||
			(collisionX && collisionZ2) || (collisionX2 && collisionZ2)))
		{
			if (_secondsSinceDamaged > 1.0f)
			{
				_secondsSinceDamaged = 0.0f;
				_health -= _damage;
				_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/hit.wav", GL_FALSE);
			}
			else
				_secondsSinceDamaged += _velocity;
		}

		return;
	}

	/** Aggro detection booleans */
	// Collision right of the hitbox?
	bool collisionX = hitboxPosition.x + _object.getAggroRadius() >= cameraPosition.x &&
		cameraPosition.x >= hitboxPosition.x;
	// Collision behind the hitbox?
	bool collisionZ = hitboxPosition.z + _object.getAggroRadius() >= cameraPosition.z &&
		cameraPosition.z >= hitboxPosition.z;
	// Collision left of the hitbox?
	bool collisionX2 = hitboxPosition.x - _object.getAggroRadius() <= cameraPosition.x &&
		cameraPosition.x <= hitboxPosition.x;
	// Collision in front of the hitbox?
	bool collisionZ2 = hitboxPosition.z - _object.getAggroRadius() <= cameraPosition.z &&
		cameraPosition.z <= hitboxPosition.z;

	Direction dir = vectorDirection(glm::vec2(cameraPosition.x - hitboxPosition.x, cameraPosition.z - hitboxPosition.z));

	if (!_object.getIsAggroed() && ((collisionX && collisionZ) || (collisionX2 && collisionZ) || 
		(collisionX && collisionZ2) || (collisionX2 && collisionZ2)))
	{
		_object.setIsAggroed(true);
		_gabenGameInputManager.getSoundEngine()->play2D("../Genesis/Audio/Life of Gaben/aggro.ogg", GL_FALSE);
	}
}

// Note the coordinate bounds within the game world and ensure proper collision resolution at those limits
void resolveWallCollisions(GLfloat _north, GLfloat _south, GLfloat _east, GLfloat _west, genesis::InputManager &_inputManager)
{
	glm::vec3 cameraPosition = _inputManager._camera.Position;

	GLfloat southBoundary = _south - 0.5f, northBoundary = _north + 0.5f, westBoundary = _west + 0.5f, eastBoundary = _east - 0.5f;

	if (cameraPosition.z >= southBoundary)
	{
		GLfloat penetration = cameraPosition.z - southBoundary;
		_inputManager._camera.Position.z -= penetration;
	}
	else if (cameraPosition.z <= northBoundary)
	{
		GLfloat penetration = northBoundary - cameraPosition.z;
		_inputManager._camera.Position.z += penetration;
	}
	else if (cameraPosition.x >= eastBoundary)
	{
		GLfloat penetration = cameraPosition.x - eastBoundary;
		_inputManager._camera.Position.x -= penetration;
	}
	else if (cameraPosition.x <= westBoundary)
	{
		GLfloat penetration = westBoundary - cameraPosition.x;
		_inputManager._camera.Position.x += penetration;
	}
}