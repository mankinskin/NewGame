#pragma once
#include "..\Global\gl.h"
#include <vector>
#include <string>

namespace gl {
    namespace Models {

	struct Model {
	    Model()
		:meshOffset(0), meshCount(0), entityOffset(0), entityCount(0) {}
	    Model(size_t pMeshOffset, size_t pMeshCount)
		:meshOffset(pMeshOffset), meshCount(pMeshCount) {}

	    void addInstances(std::vector<size_t> pEntityIDs);
	    size_t meshOffset = 0;
	    size_t meshCount = 0;
	    size_t entityOffset = 0;
	    size_t entityCount = 0;
	};
	void revalidateModelMeshOffsets();
	void revalidateModelEntityOffsets();
	void addModel(std::string pFilename);
	Model& getModel(size_t pID);
	const size_t MAX_MODELS = 100;
	void setupMeshShader();
	size_t createMesh(size_t pIndexOffset, size_t pIndexCount, size_t pTextureID, size_t pMaterialIndex);
	size_t createModel(size_t pMeshOffset, size_t pMeshCount);

    }
}