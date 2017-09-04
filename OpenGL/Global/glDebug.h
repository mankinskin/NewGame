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