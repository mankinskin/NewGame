#pragma once
#include <glm\glm.hpp>
#include <vector>
class Scene {
	struct Mesh {
		Mesh(unsigned int pOffset, unsigned int pCount, unsigned int pMaterial = 0)
			:indexOffset(pOffset), indexCount(pCount), materialIndex(pMaterial){}
		unsigned indexOffset;
		unsigned indexCount;
		unsigned materialIndex;
	};
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
	};
	std::vector<Mesh> allMeshes;
	std::vector<unsigned int> allIndices;
	std::vector<glm::vec3> allPositions;
	std::vector<glm::vec3> allNormals;
	std::vector<glm::vec2> allUVCoords;
	unsigned int newMesh(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices);
};