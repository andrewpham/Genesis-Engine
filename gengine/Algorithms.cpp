#include "Algorithms.h"

namespace genesis {

	void computeTangentBasis(
		// inputs
		std::vector<glm::vec3> & positions,
		std::vector<glm::vec2> & texCoords,
		std::vector<glm::vec3> & normals,
		// outputs
		std::vector<glm::vec3> & tangents,
		std::vector<glm::vec3> & bitangents
		) 
	{
		for (unsigned int i = 0; i<positions.size(); i += 3) 
		{
			// Shortcuts for positions
			glm::vec3 & v0 = positions[i + 0];
			glm::vec3 & v1 = positions[i + 1];
			glm::vec3 & v2 = positions[i + 2];

			// Shortcuts for texture coordinates
			glm::vec2 & uv0 = texCoords[i + 0];
			glm::vec2 & uv1 = texCoords[i + 1];
			glm::vec2 & uv2 = texCoords[i + 2];

			// Edges of the triangle : postion delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			// Set the same tangent for all three vertices of the triangle.
			// They will be merged later, in vboindexer.cpp
			tangents.push_back(tangent);
			tangents.push_back(tangent);
			tangents.push_back(tangent);

			// Same thing for binormals
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
		}
		// See "Going Further"
		for (unsigned int i = 0; i<positions.size(); i += 1)
		{
			glm::vec3 & n = normals[i];
			glm::vec3 & t = tangents[i];
			glm::vec3 & b = bitangents[i];

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness
			if (glm::dot(glm::cross(n, t), b) < 0.0f)
				t = t * -1.0f;
		}
	}
}
