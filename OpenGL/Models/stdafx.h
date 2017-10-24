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
	    Mesh(size_t pVertexOffset, size_t pVertexCount, size_t pIndexOffset, size_t pIndexCount, size_t pTextureID = 0, size_t pMaterialIndex = 0)
		:vertexOffset(pVertexOffset), vertexCount(pVertexCount), indexOffset(pIndexOffset), indexCount(pIndexCount), texture(pTextureID), materialIndex(pMaterialIndex) {}
	    size_t vertexOffset;
	    size_t vertexCount;
	    size_t indexOffset;
	    size_t indexCount;
	    size_t materialIndex;
	    size_t texture;
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
	extern std::vector<size_t> allInstanceEntities;
	extern std::vector<Mesh> allMeshes;
	extern std::vector<size_t> allIndices;
	extern std::vector<Vertex> allVertices;
	extern std::vector<Model> allModels;
	extern std::vector<Material> allMaterials;
	extern size_t meshShaderProgram;
	extern size_t normalShaderProgram;
	extern size_t meshVAO;
	extern size_t meshVBO;
	extern size_t meshIBO;
	extern size_t materialUBO;
	extern size_t entityTransformBuffer;
	extern size_t transformIndexBuffer;
	namespace Loader {
	    extern std::string MODEL_DIR;
	}
    }
}