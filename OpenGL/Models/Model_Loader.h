#pragma once
#include <assimp\Importer.hpp>
#include <vector>


namespace gl {
    namespace Models {
	namespace Loader {



	    void includeModel(std::string pFilename);
	    void loadModelFiles();
	    void loadMeshes(const aiScene * pScene);
	    void loadMaterials(const aiScene * pScene);
	    void loadModelFile(std::string pFilename);

	    void setModelDirectory(std::string&& pDirectory);
	    void resetModelDirectory();


	}
    }
}