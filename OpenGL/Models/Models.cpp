#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Models.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#include "../BaseGL/Shader/Shader.h"
#define DEFAULT_MODEL_DIR "..//assets//models//"



std::vector<std::string> gl::Models::usedModels;
std::vector<size_t> gl::Models::allInstanceEntities;
std::vector<gl::Models::Mesh> gl::Models::allMeshes;
std::vector<size_t> gl::Models::allIndices;
std::vector<gl::Models::Vertex> gl::Models::allVertices;
std::vector<gl::Models::Model> gl::Models::allModels;
std::vector<gl::Models::Material> gl::Models::allMaterials;
size_t gl::Models::meshShaderProgram = 0;
size_t gl::Models::normalShaderProgram = 0;
size_t gl::Models::meshVAO = 0;
size_t gl::Models::meshVBO = 0;
size_t gl::Models::meshIBO = 0;
size_t gl::Models::materialUBO = 0;
size_t gl::Models::entityTransformBuffer;
size_t gl::Models::transformIndexBuffer;

void gl::Models::addModel(std::string pFilename)
{
    auto it = std::find(usedModels.begin(), usedModels.end(), pFilename);
    if (it == usedModels.end()) {

    }
    else {
	App::Debug::pushError("Could not find Model: " + Loader::MODEL_DIR + pFilename + " !");
    }
}

gl::Models::Model & gl::Models::getModel(size_t pID)
{
    return allModels[pID];
}

void gl::Models::setupMeshShader()
{
    Shader::bindUniformBufferToShader(meshShaderProgram, generalUniformBuffer, "GeneralUniformBuffer");
    Shader::bindUniformBufferToShader(meshShaderProgram, entityTransformBuffer, "EntityTransformBuffer");
    Shader::bindUniformBufferToShader(meshShaderProgram, transformIndexBuffer, "TransformIndexBuffer");
}

size_t gl::Models::createMesh(size_t pIndexOffset, size_t pIndexCount, size_t pTextureID, size_t pMaterialIndex)
{
    allMeshes.push_back(Mesh(pIndexOffset, pIndexCount, pTextureID, pMaterialIndex));
    return allMeshes.size() - 1;
}


size_t gl::Models::createModel(size_t pMeshOffset, size_t pMeshCount)
{
    allModels.push_back(Model(pMeshOffset, pMeshCount));
    return allModels.size() - 1;
}

void gl::Models::revalidateModelEntityOffsets()
{
    size_t offs = 0;
    for (size_t mod = 0; mod < allModels.size(); ++mod) {
	allModels[mod].entityOffset = offs;
	offs += allModels[mod].entityCount;
    }
}

void gl::Models::revalidateModelMeshOffsets()
{
    size_t offs = 0;
    for (size_t mod = 0; mod < allModels.size(); ++mod) {
	allModels[mod].meshOffset = offs;
	offs += allModels[mod].meshCount;
    }
}

void gl::Models::Model::addInstances(std::vector<size_t> pEntityIDs) {
    if (entityCount == 0) {
	entityOffset = allInstanceEntities.size();
    }
    entityCount += pEntityIDs.size();
    allInstanceEntities.insert(allInstanceEntities.begin() + entityOffset, pEntityIDs.begin(), pEntityIDs.end());
}