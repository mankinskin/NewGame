#pragma once
#include "..\Global\gl.h"
#include <vector>
#include <string>

namespace gl {
	namespace Models {
		struct Geometry {
			Geometry() {}
			Geometry(unsigned int pOffset, unsigned int pCount)
				:indexOffset(pOffset), indexCount(pCount) {}
			unsigned indexOffset;
			unsigned indexCount;
		};
		struct Material {
			float amb_ref = 1.0f;
			float diff_ref = 0.5f;
			float spec_ref = 0.0f;
			float shine = 1.0f;
		};
		struct Mesh {
			Mesh(Geometry pGeometry, unsigned pTextureID = 0, unsigned pMaterialIndex = 0)
				:geometry(pGeometry), texture(pTextureID), materialIndex(pMaterialIndex) {}
			Geometry geometry;
			unsigned materialIndex;
			unsigned texture;
		};
		struct Model {
			Model()
				:meshOffset(0), meshCount(0), entityOffset(0), entityCount(0) {}
			Model(unsigned pMeshOffset, unsigned pMeshCount)
				:meshOffset(pMeshOffset), meshCount(pMeshCount){}
			static void revalidateMeshOffsets();
			static void revalidateEntityOffsets();
			void addInstances(std::vector<unsigned int> pEntityIDs);
			unsigned int meshOffset = 0;
			unsigned int meshCount = 0;
			unsigned int entityOffset = 0;
			unsigned int entityCount = 0;
		};
		class Scene {
			std::vector<glm::mat4> sceneInstances;
		};

		void addModel(std::string pFilename);
		const unsigned int MAX_MODELS = 100;
		Geometry newGeometry(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices);
		
		unsigned createMesh(unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pTextureID, unsigned int pMaterialIndex);
		unsigned createMesh(Geometry pGeometry, unsigned int pTextureID, unsigned int pMaterialIndex);
		unsigned createModel(unsigned pMeshOffset, unsigned pMeshCount);
		
		
		extern std::vector<std::string> usedModels;
		extern std::vector<Model> allModels;
		extern std::vector<unsigned int> allInstanceEntities;
		extern std::vector<Material> allMaterials;
		extern std::vector<Mesh> allMeshes;
		extern std::vector<unsigned int> allIndices;
		extern std::vector<Vertex> allVertices;
	}
}