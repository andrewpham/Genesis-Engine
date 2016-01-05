#pragma once
//This file holds some global algorithms

#include <vector>
#include <glm/glm.hpp>

namespace genesis {

	extern void computeTangentBasis(
		// inputs
		std::vector<glm::vec3> & positions,
		std::vector<glm::vec2> & texCoords,
		std::vector<glm::vec3> & normals,
		// outputs
		std::vector<glm::vec3> & tangents,
		std::vector<glm::vec3> & bitangents
	);

}

