#pragma once
#include "..\Global\gl.h"
#include <vector>
#include <string>

namespace gl {
	namespace Models {
		
		struct Model {
			Model()
				:meshOffset(0), meshCount(0), entityOffset(0), entityCount(0) {}
			Model(unsigned pMeshOffset, unsigned pMeshCount)
				:meshOffset(pMeshOffset), meshCount(pMeshCount) {}
			
			void addInstances(std::vector<unsigned int> pEntityIDs);
			unsigned int meshOffset = 0;
			unsigned int meshCount = 0;
			unsigned int entityOffset = 0;
			unsigned int entityCount = 0;
		};
		void revalidateModelMeshOffsets();
		void revalidateModelEntityOffsets();
		void addModel(std::string pFilename);
		Model& getModel(unsigned int pID);
		const unsigned int MAX_MODELS = 100;
		void setupMeshShader();
		unsigned createMesh(unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pTextureID, unsigned int pMaterialIndex);
		unsigned createModel(unsigned pMeshOffset, unsigned pMeshCount);
		
	}
}