#include "Global\stdafx.h"
#include "Model.h"
#include <App\Global\Debug.h>
#define DEFAULT_MODEL_DIR "..//assets//models//"
std::string gl::ModelLoader::MODEL_DIR = DEFAULT_MODEL_DIR;
std::vector<std::string> gl::ModelLoader::usedModels;
Assimp::Importer gl::ModelLoader::imp;
using std::vector;

void gl::ModelLoader::addModel(std::string pFilename)
{
	auto it = std::find(usedModels.begin(), usedModels.end(), pFilename);
	if (it == usedModels.end()) {

	}
	else {
		App::Debug::pushError("Could not find Model: " + MODEL_DIR + pFilename + " !");
	}
}

void gl::ModelLoader::setModelDirectory(std::string pDirectory)
{
	MODEL_DIR = pDirectory;
}

void gl::ModelLoader::resetModelDirectory()
{
	MODEL_DIR = DEFAULT_MODEL_DIR;
}

void gl::ModelLoader::initModelLoader()
{
	
}
