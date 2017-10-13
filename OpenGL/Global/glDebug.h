#pragma once
#include "gl.h"
#include <vector>
#include <unordered_map>

namespace gl {
    namespace Debug {

	struct dMesh {
	    size_t offset;
	    size_t size;
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

	extern size_t vertexBuffer;
	extern size_t indexBuffer;
	extern size_t VAO;
	extern size_t lineShaderID;

	extern std::vector<LineVertex> vertices;
	extern std::vector<size_t> indices;
	extern std::vector<dMesh> debugMeshes;
	extern std::vector<int> meshFlags;
	void setDebugElement(std::string pName, int pSet);
	void toggleDebugElement(std::string pName);
	void toggleGrid();
	void toggleCoord();
	extern std::unordered_map<std::string, size_t> debugMeshLookup;
	void generateDebugGrid(std::string pName, float pTileSize, size_t pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha);
	void generateDebugGrid(std::string pName, float pTileSizeX, float pTileSizeY, size_t pTileCountX, size_t pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha);
	void initDebugVAOs();
	void init();
	void drawGrid();
	void initCoordinateSystem(std::string pName);
	void initDebugShader();
	void getGLError(string pReferenceMessage);
    }
}