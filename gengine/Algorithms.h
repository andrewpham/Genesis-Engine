#pragma once
//This file holds some global algorithms

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

namespace genesis {

	extern void computeTangentBasis(
		glm::vec3 &pos1, glm::vec3 &pos2, glm::vec3 &pos3, glm::vec3 &pos4,
		glm::vec2 &uv1, glm::vec2 &uv2, glm::vec2 &uv3, glm::vec2 &uv4, 
		glm::vec3 &nm, 
		glm::vec3 &tangent1, glm::vec3 &bitangent1, 
		glm::vec3 &tangent2, glm::vec3 &bitangent2
	);

}

