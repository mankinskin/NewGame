#include "..\Global\stdafx.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#include <assimp\scene.h>

#define DEFAULT_MODEL_DIR "..//assets//models//"
std::string gl::Model::Loader::MODEL_DIR = DEFAULT_MODEL_DIR;
Assimp::Importer gl::Model::Loader::imp;


void gl::Model::Loader::setModelDirectory(std::string pDirectory)
{
	MODEL_DIR = pDirectory;
}

void gl::Model::Loader::resetModelDirectory()
{
	MODEL_DIR = DEFAULT_MODEL_DIR;
}

void gl::Model::Loader::init()
{

}