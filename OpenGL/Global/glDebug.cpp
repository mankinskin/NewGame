#include "stdafx.h"
#include "glDebug.h"
#include <App\Global\Debug.h>
#include "../BaseGL/Shader/Shader.h"
#include "..\Camera.h"
#include "../BaseGL/VAO.h"
#include "../BaseGL/Framebuffer.h"

size_t gl::Debug::vertexBuffer;
size_t gl::Debug::indexBuffer;
std::vector<gl::Debug::LineVertex> gl::Debug::vertices;
std::vector<size_t> gl::Debug::indices;
size_t gl::Debug::VAO = 0;
size_t gl::Debug::lineShaderID = 0;
std::vector<gl::Debug::dMesh> gl::Debug::debugMeshes;
std::unordered_map<std::string, size_t> gl::Debug::debugMeshLookup;
std::vector<int> gl::Debug::meshFlags;

void gl::Debug::getGLError(string pReferenceMessage) {
    while (GLenum error = glGetError()) {
	App::Debug::pushError("!!!\nOPENGL ERROR at " + pReferenceMessage + "\nopenGL Log:\n" + (char *)(gluErrorString(error)) + "\n", App::Debug::Error::Fatal);
    }
}

void gl::Debug::setDebugElement(std::string pName, int pSet)
{
    auto it = debugMeshLookup.find(pName);
    if (it != debugMeshLookup.end()) {
	meshFlags[it->second] = pSet;
    }
    else {
	App::Debug::pushError("glDebug.c: could not find Debug Element " + pName + "!\n");
    }
}

void gl::Debug::toggleDebugElement(std::string pName)
{
    auto it = debugMeshLookup.find(pName);
    if (it != debugMeshLookup.end()) {
	meshFlags[it->second] = !meshFlags[it->second];
    }
    else {
	App::Debug::pushError("glDebug.c: could not find Debug Element " + pName + "!\n");
    }
}

void gl::Debug::toggleGrid()
{
    toggleDebugElement("grid1.0");
    toggleDebugElement("grid0.5");
}

void gl::Debug::toggleCoord()
{
    toggleDebugElement("coord");
}

void gl::Debug::generateDebugGrid(std::string pName, float pTileSize, size_t pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha)
{
    generateDebugGrid(pName, pTileSize, pTileSize, pTileCount, pTileCount, pColorR, pColorG, pColorB, pAlpha);
}

void gl::Debug::generateDebugGrid(std::string pName, float pTileSizeX, float pTileSizeY, size_t pTileCountX, size_t pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha)
{
    //generate grid vertices and indices
    int vertexCount = ((pTileCountX - 1) * 2 + (pTileCountY + 1) * 2);
    int indexCount = vertexCount + 4;
    std::vector<LineVertex> verts;
    std::vector<size_t> inds;
    if (vertexCount < 4) {
	return;
    }
    verts.resize(vertexCount);
    inds.resize(indexCount);
    vertexCount = 0;
    indexCount = 0;
    float xMin = -0.5f*pTileSizeX*((float)pTileCountX);
    float yMin = -0.5f*pTileSizeY*((float)pTileCountY);

    size_t vertexOffset = vertices.size();
    size_t indexOffset = indices.size();
    //horizontals
    for (size_t y = 0; y <= pTileCountY; ++y) {

	for (size_t x = 0; x < 2; ++x) {

	    verts[y * 2 + x] = LineVertex(xMin + pTileSizeX * pTileCountX * x, 0.0f, yMin + pTileSizeY * y, 1.0f, pColorR, pColorG, pColorB, pAlpha);
	    inds[y * 2 + x] = vertexOffset + y * 2 + x;
	}
	vertexCount += 2;
	indexCount += 2;
    }
    //vertical borders 
    inds[indexCount + 0] = vertexOffset; // first vertex
    inds[indexCount + 1] = vertexOffset + vertexCount - 2; //one before last vertex
    inds[indexCount + 2] = vertexOffset + 1;//second vertex
    inds[indexCount + 3] = vertexOffset + vertexCount - 1;//last vertex
    indexCount += 4;

    size_t v = 0;
    size_t ind = 0;
    //verticals
    for (size_t x = 0; x < pTileCountX - 1; ++x) {
	for (size_t y = 0; y < 2; ++y) {
	    verts[vertexCount + x * 2 + y] = LineVertex(xMin + (x + 1)*pTileSizeX, 0.0f, yMin + y * pTileSizeY * pTileCountY, 1.0f, pColorR, pColorG, pColorB, pAlpha);
	    inds[indexCount + x * 2 + y] = vertexOffset + vertexCount + x * 2 + y;
	}
	v += 2;
	ind += 2;
    }
    indexCount += ind;
    //add vertices and indices to debug geometry arrays
    dMesh dmesh;
    dmesh.offset = indexOffset;
    dmesh.size = indexCount;
    debugMeshLookup.insert(std::pair<std::string, size_t>(pName, debugMeshes.size()));
    debugMeshes.push_back(dmesh);
    meshFlags.push_back(1);
    vertices.insert(vertices.end(), verts.begin(), verts.end());
    indices.insert(indices.end(), inds.begin(), inds.end());

    getGLError("generateDebugGrid():");
}

void gl::Debug::initDebugVAOs()
{
    glCreateVertexArrays(1, &VAO);
    glVertexArrayVertexBuffer(VAO, 0, vertexBuffer + 1, 0, sizeof(LineVertex));
    VAO::setVertexAttrib(VAO, 0, 0, 4, GL_FLOAT, offsetof(LineVertex, pos));
    VAO::setVertexAttrib(VAO, 0, 1, 4, GL_FLOAT, offsetof(LineVertex, color));
    glVertexArrayElementBuffer(VAO, indexBuffer + 1);


    getGLError("initDebugVAOs():");
}

void gl::Debug::init()
{
    //Debug::generateDebugGrid("grid1.0", 1.0f, 1000, 1.0f, 1.0f, 1.0f, 0.3f);
    Debug::generateDebugGrid("grid0.5", 0.5f, 2000, 1.0f, 1.0f, 1.0f, 0.1f);

    Debug::initCoordinateSystem("coord");

    vertexBuffer = VAO::createStorage(vertices.size() * sizeof(LineVertex), &vertices[0], 0);
    indexBuffer = VAO::createStorage(indices.size() * sizeof(size_t), &indices[0], 0);
    initDebugVAOs();

    getGLError("gl::Debug::init():");
}

void gl::Debug::drawGrid()
{
    glDepthMask(0);
    glBindVertexArray(VAO);
    Shader::use(lineShaderID);
    for (size_t m = 0; m < debugMeshes.size(); ++m) {
	if (meshFlags[m]) {
	    size_t offset = debugMeshes[m].offset;
	    size_t count = debugMeshes[m].size;
	    glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(size_t)));
	}
    }

    Shader::unuse();
    glBindVertexArray(0);
    glDepthMask(1);
    getGLError("drawGrid():");
}

void gl::Debug::initCoordinateSystem(std::string pName)
{
    std::vector<LineVertex> verts;
    std::vector<size_t> inds;
    verts.resize(9);
    inds.resize(6 * 2);
    size_t vertexOffset = vertices.size();
    size_t indexOffset = indices.size();



    verts[0] = LineVertex(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    verts[1] = LineVertex(-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    verts[2] = LineVertex(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

    verts[3] = LineVertex(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    verts[4] = LineVertex(0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    verts[5] = LineVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    verts[6] = LineVertex(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    verts[7] = LineVertex(0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    verts[8] = LineVertex(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    for (size_t i = 0; i < 3; ++i) {
	inds[i * 4] = vertexOffset + i * 3;
	inds[i * 4 + 1] = vertexOffset + 1 + i * 3;
	inds[i * 4 + 2] = vertexOffset + i * 3;
	inds[i * 4 + 3] = vertexOffset + 2 + i * 3;
    }

    dMesh dmesh;
    dmesh.offset = indexOffset;
    dmesh.size = 12;
    debugMeshLookup.insert(std::pair<std::string, size_t>(pName, debugMeshes.size()));
    debugMeshes.push_back(dmesh);
    meshFlags.push_back(1);
    vertices.insert(vertices.end(), verts.begin(), verts.end());
    indices.insert(indices.end(), inds.begin(), inds.end());
}

void gl::Debug::initDebugShader()
{
    lineShaderID = Shader::newProgram("debugLine", Shader::createModule("gridShader.vert"), Shader::createModule("gridShader.frag"));
    Shader::addVertexAttribute(lineShaderID, "position", 0);
    Shader::addVertexAttribute(lineShaderID, "color", 1);
}
