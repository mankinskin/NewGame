#pragma once
#include <assimp\Importer.hpp>



namespace gl {
	namespace Models {
		namespace Loader {
			void init();
			void setModelDirectory(std::string pDirectory);
			void resetModelDirectory();

			extern std::string MODEL_DIR;
			
			extern Assimp::Importer imp;
		}
	}
}