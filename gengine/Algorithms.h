#pragma once
#define _CRT_SECURE_NO_DEPRECATE
//This file holds some global algorithms

#include <GL/glew.h>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <glm/glm.hpp>

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace genesis {

	extern void computeTangentBasis(
		glm::vec3 &pos1, glm::vec3 &pos2, glm::vec3 &pos3, glm::vec3 &pos4,
		glm::vec2 &uv1, glm::vec2 &uv2, glm::vec2 &uv3, glm::vec2 &uv4, 
		glm::vec3 &nm, 
		glm::vec3 &tangent1, glm::vec3 &bitangent1, 
		glm::vec3 &tangent2, glm::vec3 &bitangent2
	);

	bool loadOBJ(
		const char *path,
		std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec2> &out_uvs,
		std::vector<glm::vec3> &out_normals
	);



	bool loadAssImp(
		const char *path,
		std::vector<unsigned short> &indices,
		std::vector<glm::vec3> &vertices,
		std::vector<glm::vec2> &uvs,
		std::vector<glm::vec3> &normals
	);

}

