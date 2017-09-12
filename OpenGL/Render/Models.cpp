#include "..\Global\stdafx.h"
#include "Models.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#define DEFAULT_MODEL_DIR "..//assets//models//"


std::vector<std::string> gl::Models::usedModels;
std::vector<gl::Models::Model> gl::Models::allModels;
std::vector<unsigned int> gl::Models::allInstanceEntities;
std::vector<gl::Models::Material> gl::Models::allMaterials;
std::vector<gl::Models::Mesh> gl::Models::allMeshes;
std::vector<unsigned int> gl::Models::allIndices;
std::vector<gl::Vertex> gl::Models::allVertices;

void gl::Models::addModel(std::string pFilename)
{
	auto it = std::find(usedModels.begin(), usedModels.end(), pFilename);
	if (it == usedModels.end()) {

	}
	else {
		App::Debug::pushError("Could not find Model: " + Loader::MODEL_DIR + pFilename + " !");
	}
}

unsigned gl::Models::createMesh(unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pTextureID, unsigned int pMaterialIndex)
{
	allMeshes.push_back(Mesh(Geometry(pIndexOffset, pIndexCount), pTextureID, pMaterialIndex));
	return allMeshes.size() - 1;
}

unsigned gl::Models::createMesh(Geometry pGeometry, unsigned int pTextureID, unsigned int pMaterialIndex)
{
	allMeshes.push_back(Mesh(pGeometry, pTextureID, pMaterialIndex));
	return allMeshes.size() - 1;
}

unsigned gl::Models::createModel(unsigned pMeshOffset, unsigned pMeshCount)
{
	allModels.push_back(Model(pMeshOffset, pMeshCount));
	return allModels.size()-1;
}

void gl::Models::Model::revalidateEntityOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].entityOffset = offs;
		offs += allModels[mod].entityCount;
	}
}

void gl::Models::Model::revalidateMeshOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].meshOffset = offs;
		offs += allModels[mod].meshCount;
	}
}

gl::Models::Geometry gl::Models::newGeometry(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices)
{
	Geometry geom(allIndices.size(), pIndices.size());
	allIndices.insert(allIndices.end(), pIndices.begin(), pIndices.end());
	allVertices.insert(allVertices.end(), pVerts.begin(), pVerts.end());
	return geom;
}

void gl::Models::Model::addInstances(std::vector<unsigned int> pEntityIDs) {
	if (entityCount == 0) {
		entityOffset = allInstanceEntities.size();
	}
	entityCount += pEntityIDs.size();
	allInstanceEntities.insert(allInstanceEntities.begin() + entityOffset, pEntityIDs.begin(), pEntityIDs.end());
}