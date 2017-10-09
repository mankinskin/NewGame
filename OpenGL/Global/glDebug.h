#pragma once
#include "gl.h"
#include <vector>
#include <unordered_map>

namespace gl {
	namespace Debug {

		struct dMesh {
			unsigned int offset;
			unsigned int size;
			glm::vec3 position = glm::vec3();
		};

		struct LineVertex {
			LineVertex()
				:pos(glm::vec4()), color(glm::vec4()) {}
			LineVertex(float pPosX, float pPosY, float pPosZ, float pPosW, float pR, float pG, float pB, float pA)
				:pos(glm::vec4(pPosX, pPosY, pPosZ, pPosW)), color(glm::vec4(pR, pG, pB, pA)) {}
			glm::vec4 pos;
			glm::vec4 color;
		};

		extern unsigned int vertexBuffer;
		extern unsigned int indexBuffer;
		extern unsigned int VAO;
		extern unsigned int lineShaderID;
		
		extern std::vector<LineVertex> vertices;
		extern std::vector<unsigned int> indices;
		extern std::vector<dMesh> debugMeshes;
		extern std::vector<int> meshFlags;
		void setDebugElement(std::string pName, int pSet);
		void toggleDebugElement(std::string pName);
		void toggleGrid();
		void toggleCoord();
		extern std::unordered_map<std::string, unsigned int> debugMeshLookup;
		void generateDebugGrid(std::string pName, float pTileSize, unsigned int pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha);
		void generateDebugGrid(std::string pName, float pTileSizeX, float pTileSizeY, unsigned int pTileCountX, unsigned int pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha);
		void initDebugVAOs();
		void init();
		void drawGrid();
		void initCoordinateSystem(std::string pName);
		void initDebugShader();
		void getGLError(string pReferenceMessage);
	}
}