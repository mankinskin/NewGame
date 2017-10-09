#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace gl {
	namespace Models {
		struct Material {
			float amb_ref = 1.0f;
			float diff_ref = 0.5f;
			float spec_ref = 0.0f;
			float shine = 1.0f;
		};
		struct Mesh {
			Mesh(unsigned pVertexOffset, unsigned pVertexCount, unsigned pIndexOffset, unsigned pIndexCount, unsigned pTextureID = 0, unsigned pMaterialIndex = 0)
				:vertexOffset(pVertexOffset), vertexCount(pVertexCount), indexOffset(pIndexOffset), indexCount(pIndexCount), texture(pTextureID), materialIndex(pMaterialIndex) {}
			unsigned vertexOffset;
			unsigned vertexCount;
			unsigned indexOffset;
			unsigned indexCount;
			unsigned materialIndex;
			unsigned texture;
		};
		struct Vertex {
			Vertex() {}
			Vertex(float pPosX, float pPosY, float pPosZ, float pNormX, float pNormY, float pNormZ, float pUVCoordS, float pUVCoordT)
				:pos(glm::vec3(pPosX, pPosY, pPosZ)), normal(glm::vec3(pNormX, pNormY, pNormZ)), uv(glm::vec2(pUVCoordS, pUVCoordT)) {}
			Vertex(glm::vec3 pPos, glm::vec3 pNormal, glm::vec2 pUV)
				:pos(pPos), normal(pNormal), uv(pUV) {}
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 uv;
		};

		struct Model;
		struct Scene {
			std::vector<glm::mat4> sceneInstances;
		};
		extern std::vector<std::string> usedModels;
		extern std::vector<unsigned int> allInstanceEntities;
		extern std::vector<Mesh> allMeshes;
		extern std::vector<unsigned int> allIndices;
		extern std::vector<Vertex> allVertices;
		extern std::vector<Model> allModels;
		extern std::vector<Material> allMaterials;
		extern unsigned int meshShaderProgram;
		extern unsigned int normalShaderProgram;
		extern unsigned int meshVAO;
		extern unsigned int meshVBO;
		extern unsigned int meshIBO;
		extern unsigned int materialUBO;
		extern unsigned int entityTransformBuffer;
		extern unsigned int transformIndexBuffer;
		namespace Loader {
			extern std::string MODEL_DIR;
		}
	}
}