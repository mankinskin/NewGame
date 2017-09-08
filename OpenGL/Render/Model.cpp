#include "..\Global\stdafx.h"
#include "Model.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#define DEFAULT_MODEL_DIR "..//assets//models//"


std::vector<std::string> gl::Model::usedModels;
std::vector<gl::Model::Model> gl::Model::allModels;
std::vector<unsigned int> gl::Model::allInstanceEntities;
std::vector<gl::Model::Material> gl::Model::allMaterials;
std::vector<gl::Model::Mesh> gl::Model::allMeshes;
std::vector<unsigned int> gl::Model::allIndices;
std::vector<gl::Vertex> gl::Model::allVertices;

void gl::Model::addModel(std::string pFilename)
{
	auto it = std::find(usedModels.begin(), usedModels.end(), pFilename);
	if (it == usedModels.end()) {

	}
	else {
		App::Debug::pushError("Could not find Model: " + Loader::MODEL_DIR + pFilename + " !");
	}
}



unsigned gl::Model::createMesh(unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pTextureID, unsigned int pMaterialIndex)
{
	allMeshes.push_back(Mesh(Geometry(pIndexOffset, pIndexCount), pTextureID, pMaterialIndex));
	return allMeshes.size() - 1;
}

unsigned gl::Model::createMesh(Geometry pGeometry, unsigned int pTextureID, unsigned int pMaterialIndex)
{
	allMeshes.push_back(Mesh(pGeometry, pTextureID, pMaterialIndex));
	return allMeshes.size() - 1;
}

unsigned gl::Model::createModel(unsigned pMeshOffset, unsigned pMeshCount)
{
	allModels.push_back(Model(pMeshOffset, pMeshCount));
	return allModels.size()-1;
}

void gl::Model::revalidateEntityOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].entityOffset = offs;
		offs += allModels[mod].entityCount;
	}
}

void gl::Model::revalidateMeshOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].meshOffset = offs;
		offs += allModels[mod].meshCount;
	}
}

gl::Model::Geometry gl::Model::newGeometry(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices)
{
	Geometry geom(allIndices.size(), pIndices.size());
	allIndices.insert(allIndices.end(), pIndices.begin(), pIndices.end());
	allVertices.insert(allVertices.end(), pVerts.begin(), pVerts.end());
	return geom;
}

void gl::Model::addModelInstances(unsigned int pModelIndex, std::vector<unsigned int> pEntityIDs) {
	Model& model = allModels[pModelIndex];
	if (model.entityCount == 0) {
		model.entityOffset = allInstanceEntities.size();
	}
	model.entityCount += pEntityIDs.size();
	allInstanceEntities.insert(allInstanceEntities.begin() + model.entityOffset + model.entityCount, pEntityIDs.begin(), pEntityIDs.end());
	
}