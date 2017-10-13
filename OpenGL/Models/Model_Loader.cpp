#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "Models.h"
#define DEFAULT_MODEL_DIR "..//assets//models//"

std::string gl::Models::Loader::MODEL_DIR = DEFAULT_MODEL_DIR;
Assimp::Importer imp = Assimp::Importer();
std::vector<std::string> modelLoadBuffer;



void gl::Models::Loader::includeModel(std::string pFilename)
{
    modelLoadBuffer.push_back(pFilename);
}

void gl::Models::Loader::loadModelFiles()
{
    allModels.reserve(allModels.size() + modelLoadBuffer.size());
    for (std::string& file : modelLoadBuffer) {
	loadModelFile(file);
    }
    allModels.shrink_to_fit();
}

void gl::Models::Loader::loadMeshes(const aiScene* pScene) {

    size_t vertexCount = 0;
    size_t faceCount = 0;

    //collect mesh information
    for (size_t m = 0; m < pScene->mNumMeshes; ++m) {
	aiMesh* mesh = pScene->mMeshes[m];
	vertexCount += mesh->mNumVertices;
	faceCount += mesh->mNumFaces;
    }

    allVertices.reserve(allVertices.size() + vertexCount);
    allIndices.reserve(allIndices.size() + faceCount * 3);
    allMeshes.reserve(allMeshes.size() + pScene->mNumMeshes);
    vertexCount = 0;
    faceCount = 0;//used as offsets now

    for (size_t m = 0; m < pScene->mNumMeshes; ++m) {
	aiMesh* mesh = pScene->mMeshes[m];

	for (size_t v = 0; v < mesh->mNumVertices; ++v) {
	    aiVector3D pos = mesh->mVertices[v];
	    aiVector3D normal = mesh->mNormals[v];
	    aiVector3D uv = mesh->mTextureCoords[0][v];
	    allVertices.push_back(Vertex(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uv.x, uv.y));
	}
	for (size_t f = 0; f < mesh->mNumFaces; ++f) {
	    aiFace face = mesh->mFaces[f];
	    for (size_t fi = 0; fi < 3; ++fi) {
		allIndices.push_back(face.mIndices[fi]);
	    }

	}
	allMeshes.push_back(Mesh(vertexCount, mesh->mNumVertices, faceCount * 3, mesh->mNumFaces * 3, 0, 0));// allMaterials.size() + mesh->mMaterialIndex));
	vertexCount += mesh->mNumVertices;
	faceCount += mesh->mNumFaces;
    }
}

void gl::Models::Loader::loadMaterials(const aiScene* pScene) {

    allMaterials.reserve(allMaterials.size() + pScene->mNumMaterials);

    for (size_t m = 0; m < pScene->mNumMaterials; ++m) {
	aiMaterial* mat = pScene->mMaterials[m];
	//continue... 

    }
}

void gl::Models::Loader::loadModelFile(std::string pFilename)
{

    const aiScene* scene = imp.ReadFile(MODEL_DIR + pFilename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);
    const char* err = imp.GetErrorString();
    if (err[0] != '\0') {
	App::Debug::pushError(err);
    }
    if (scene == nullptr) {
	App::Debug::pushError("scene was nullptr!");
    }
    Model model;
    model.meshOffset = allMeshes.size();
    model.meshCount = scene->mNumMeshes;
    loadMeshes(scene);
    //loadMaterials(scene);

    allModels.push_back(model);
}

void gl::Models::Loader::setModelDirectory(std::string&& pDirectory)
{
    MODEL_DIR = pDirectory;
}

void gl::Models::Loader::resetModelDirectory()
{
    MODEL_DIR = DEFAULT_MODEL_DIR;
}

