#pragma once
#include <assimp\Importer.hpp>
#include <vector>


namespace gl {
	namespace Models {
		namespace Loader {

                        

                        extern std::vector<std::string> modelLoadBuffer;

                        void includeModel(std::string pFilename);
                        void loadModelFiles();
                        void loadMeshes(const aiScene * pScene);
                        void loadMaterials(const aiScene * pScene);
                        void loadModelFile(std::string pFilename);
                        
			void setModelDirectory(std::string&& pDirectory);
			void resetModelDirectory();

			extern std::string MODEL_DIR;
			
			extern Assimp::Importer imp;
		}
	}
}