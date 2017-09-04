#pragma once
#include "Global\gl.h"
#include <vector>
#include <string>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
namespace gl {
	namespace ModelLoader {
		void initModelLoader();
		void addModel(std::string pFilename);
		void setModelDirectory(std::string pDirectory);
		void resetModelDirectory();

		extern std::string MODEL_DIR;
		extern std::vector<std::string> usedModels;
		extern Assimp::Importer imp;
	}
}