#include "Algorithms.h"

namespace genesis {

	void computeTangentBasis(
		glm::vec3 &pos1, glm::vec3 &pos2, glm::vec3 &pos3, glm::vec3 &pos4,
		glm::vec2 &uv1, glm::vec2 &uv2, glm::vec2 &uv3, glm::vec2 &uv4,
		glm::vec3 &nm,
		glm::vec3 &tangent1, glm::vec3 &bitangent1,
		glm::vec3 &tangent2, glm::vec3 &bitangent2
		) 
	{
		// - triangle 1
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// - triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);
	}

	bool loadOBJ(
		const char * path,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals
		) {
		printf("Loading OBJ file %s...\n", path);

		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;


		FILE * file = fopen(path, "r");
		if (file == NULL) {
			printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
			getchar();
			return false;
		}

		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

					   // else : parse lineHeader

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser :-( Try exporting with other options\n");
					return false;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else {
				// Probably a comment, eat up the rest of the line
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}

		}

		// For each vertex of each triangle
		for (unsigned int i = 0; i<vertexIndices.size(); i++) {

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			glm::vec3 normal = temp_normals[normalIndex - 1];

			// Put the attributes in buffers
			out_vertices.push_back(vertex);
			out_uvs.push_back(uv);
			out_normals.push_back(normal);

		}

		return true;
	}

	bool loadAssImp(
		const char * path,
		std::vector<unsigned short> & indices,
		std::vector<glm::vec3> & vertices,
		std::vector<glm::vec2> & uvs,
		std::vector<glm::vec3> & normals
		) {

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
		if (!scene) {
			fprintf(stderr, importer.GetErrorString());
			getchar();
			return false;
		}
		const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

												// Fill vertices positions
		vertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i<mesh->mNumVertices; i++) {
			aiVector3D pos = mesh->mVertices[i];
			vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
		}

		// Fill vertices texture coordinates
		uvs.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i<mesh->mNumVertices; i++) {
			aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
			uvs.push_back(glm::vec2(UVW.x, UVW.y));
		}

		// Fill vertices normals
		normals.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i<mesh->mNumVertices; i++) {
			aiVector3D n = mesh->mNormals[i];
			normals.push_back(glm::vec3(n.x, n.y, n.z));
		}


		// Fill face indices
		indices.reserve(3 * mesh->mNumFaces);
		for (unsigned int i = 0; i<mesh->mNumFaces; i++) {
			// Assume the model has only triangles.
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[1]);
			indices.push_back(mesh->mFaces[i].mIndices[2]);
		}

		// The "scene" pointer will be deleted automatically by "importer"

	}

}
