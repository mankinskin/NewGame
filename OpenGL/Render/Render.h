#pragma once
#include <glm\glm.hpp>
#include <vector>

namespace gl {
	namespace Render {
		struct Vertex {
			Vertex(){}
			Vertex(float pPosX, float pPosY, float pPosZ, float pNormX, float pNormY, float pNormZ, float pUVCoordS, float pUVCoordT)
				:pos(glm::vec3(pPosX, pPosY, pPosZ)), normal(glm::vec3(pNormX, pNormY, pNormZ)), uv(glm::vec2(pUVCoordS, pUVCoordT)){}
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 uv;
		};
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
			Mesh(Geometry pGeometry, unsigned pMaterialIndex = 0)
				:geometry(pGeometry), materialIndex(pMaterialIndex){}
			Geometry geometry;
			unsigned materialIndex;
		};
		struct Model {
			unsigned int meshOffset;
			unsigned int meshCount;
		};
		class Scene {
			std::vector<glm::mat4> sceneInstances;
		};

		void initMeshVAO();
		void initMeshShader();
		void fillMeshVAO();
		void storeMaterials();
		void render();
		Geometry newGeometry(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices);
		extern unsigned int meshShaderProgram;
		extern unsigned int meshVAO;
		extern unsigned int meshVBO;
		extern unsigned int meshIBO;
		extern unsigned int materialUBO;
		extern std::vector<Model> allModels;
		extern std::vector<Material> allMaterials;
		extern std::vector<Mesh> allMeshes;
		extern std::vector<unsigned int> allIndices;
		extern std::vector<Vertex> allVertices;
	}
}