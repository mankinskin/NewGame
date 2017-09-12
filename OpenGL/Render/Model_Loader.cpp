#include "..\Global\stdafx.h"
#include "Model_Loader.h"
#include <App\Global\Debug.h>
#include <assimp\scene.h>

#define DEFAULT_MODEL_DIR "..//assets//models//"
std::string gl::Models::Loader::MODEL_DIR = DEFAULT_MODEL_DIR;
Assimp::Importer gl::Models::Loader::imp;


void gl::Models::Loader::setModelDirectory(std::string pDirectory)
{
	MODEL_DIR = pDirectory;
}

void gl::Models::Loader::resetModelDirectory()
{
	MODEL_DIR = DEFAULT_MODEL_DIR;
}

void gl::Models::Loader::init()
{

}