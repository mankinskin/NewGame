#include "Scene.h"

unsigned int Scene::newMesh(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices)
{
	const unsigned int prev_vert_count = allPositions.size();
	const unsigned int vert_count = pVerts.size();
	allPositions.resize(prev_vert_count + vert_count);
	allNormals.resize(prev_vert_count + vert_count);
	allUVCoords.resize(prev_vert_count + vert_count);

	for (unsigned int v = 0; v < vert_count; ++v) {
		Vertex& vert = pVerts[v];
		allPositions[prev_vert_count + v] = vert.pos;
		allNormals[prev_vert_count + v] = vert.normal;
		allUVCoords[prev_vert_count + v] = vert.uv;
	}

	Mesh mesh(allIndices.size(), pIndices.size());
	allIndices.insert(allIndices.end(), pIndices.begin(), pIndices.end());
	allMeshes.push_back(mesh);
	return allMeshes.size()-1;
}
