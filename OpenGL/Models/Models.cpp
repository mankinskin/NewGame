#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Models.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#include "../BaseGL/Shader/Shader.h"
#define DEFAULT_MODEL_DIR "..//assets//models//"



std::vector<std::string> gl::Models::usedModels;
std::vector<unsigned int> gl::Models::allInstanceEntities;
std::vector<gl::Models::Mesh> gl::Models::allMeshes;
std::vector<unsigned int> gl::Models::allIndices;
std::vector<gl::Models::Vertex> gl::Models::allVertices;
std::vector<gl::Models::Model> gl::Models::allModels;
std::vector<gl::Models::Material> gl::Models::allMaterials;
unsigned int gl::Models::meshShaderProgram = 0;
unsigned int gl::Models::normalShaderProgram = 0;
unsigned int gl::Models::meshVAO = 0;
unsigned int gl::Models::meshVBO = 0;
unsigned int gl::Models::meshIBO = 0;
unsigned int gl::Models::materialUBO = 0;
unsigned int gl::Models::entityTransformBuffer;
unsigned int gl::Models::transformIndexBuffer;

void gl::Models::addModel(std::string pFilename)
{
	auto it = std::find(usedModels.begin(), usedModels.end(), pFilename);
	if (it == usedModels.end()) {

	}
	else {
		App::Debug::pushError("Could not find Model: " + Loader::MODEL_DIR + pFilename + " !");
	}
}

gl::Models::Model & gl::Models::getModel(unsigned int pID)
{
	return allModels[pID];
}

void gl::Models::setupMeshShader()
{
	Shader::bindUniformBufferToShader(meshShaderProgram, generalUniformBuffer, "GeneralUniformBuffer");
	Shader::bindUniformBufferToShader(meshShaderProgram, entityTransformBuffer, "EntityTransformBuffer");
	Shader::bindUniformBufferToShader(meshShaderProgram, transformIndexBuffer, "TransformIndexBuffer");
}

unsigned gl::Models::createMesh(unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pTextureID, unsigned int pMaterialIndex)
{
	allMeshes.push_back(Mesh(pIndexOffset, pIndexCount, pTextureID, pMaterialIndex));
	return allMeshes.size() - 1;
}


unsigned gl::Models::createModel(unsigned pMeshOffset, unsigned pMeshCount)
{
	allModels.push_back(Model(pMeshOffset, pMeshCount));
	return allModels.size()-1;
}

void gl::Models::revalidateModelEntityOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].entityOffset = offs;
		offs += allModels[mod].entityCount;
	}
}

void gl::Models::revalidateModelMeshOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].meshOffset = offs;
		offs += allModels[mod].meshCount;
	}
}

void gl::Models::Model::addInstances(std::vector<unsigned int> pEntityIDs) {
	if (entityCount == 0) {
		entityOffset = allInstanceEntities.size();
	}
	entityCount += pEntityIDs.size();
	allInstanceEntities.insert(allInstanceEntities.begin() + entityOffset, pEntityIDs.begin(), pEntityIDs.end());
}