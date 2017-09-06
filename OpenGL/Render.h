#pragma once
#include "Global\gl.h"
#include <glm\glm.hpp>
#include <vector>

namespace gl {
	namespace Render {
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 uv;
		};
		struct Geometry {
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
		void fillMeshVAO();
		void storeMaterials();
		unsigned int newMesh(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices);
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