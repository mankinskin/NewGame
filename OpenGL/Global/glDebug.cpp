#include "stdafx.h"
#include "glDebug.h"
#include <App\Global\Debug.h>
#include "..\BaseGL\Shader.h"
#include "..\Camera.h"


unsigned int gl::Debug::vertexBuffer;
unsigned int gl::Debug::indexBuffer;
std::vector<gl::LineVertex> gl::Debug::vertices;
std::vector<unsigned int> gl::Debug::indices;
unsigned int gl::Debug::VAO = 0;
unsigned int gl::Debug::lineShaderID = 0;
std::vector<gl::Debug::dMesh> gl::Debug::debugMeshes;
std::unordered_map<std::string, unsigned int> gl::Debug::debugMeshLookup;
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

void gl::Debug::generateDebugGrid(std::string pName, float pTileSize, unsigned int pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha)
{
	generateDebugGrid(pName, pTileSize, pTileSize, pTileCount, pTileCount, pColorR, pColorG, pColorB, pAlpha);
}

void gl::Debug::generateDebugGrid(std::string pName, float pTileSizeX, float pTileSizeY, unsigned int pTileCountX, unsigned int pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha)
{
	//generate grid vertices and indices
	int vertexCount = ((pTileCountX-1) * 2 + (pTileCountY+1) * 2);
	int indexCount = vertexCount + 4;
	std::vector<LineVertex> verts;
	std::vector<unsigned int> inds;
	if (vertexCount < 4) {
		return;
	}
	verts.resize(vertexCount);
	inds.resize(indexCount);
	vertexCount = 0;
	indexCount = 0;
	float xMin = -0.5f*pTileSizeX*((float)pTileCountX);
	float yMin = -0.5f*pTileSizeY*((float)pTileCountY);

	unsigned int vertexOffset = vertices.size();
	unsigned int indexOffset = indices.size();
	//horizontals
	for (unsigned int y = 0; y <= pTileCountY; ++y) {

		for (unsigned int x = 0; x < 2; ++x) {
			
			verts[y * 2 + x] = LineVertex(xMin + pTileSizeX * pTileCountX * x, 0.0f, yMin + pTileSizeY * y, 1.0f, pColorR, pColorG, pColorB, pAlpha);
			inds[y * 2 + x] = vertexOffset + y * 2 + x;
		}
		vertexCount += 2;
		indexCount += 2;
	}
	//vertical borders 
	inds[indexCount + 0] = vertexOffset; // first vertex
	inds[indexCount + 1] = vertexOffset + vertexCount -2; //one before last vertex
	inds[indexCount + 2] = vertexOffset + 1;//second vertex
	inds[indexCount + 3] = vertexOffset + vertexCount - 1;//last vertex
	indexCount += 4;

	unsigned int v = 0;
	unsigned int ind = 0;
	//verticals
	for (unsigned int x = 0; x < pTileCountX-1; ++x) {
		for (unsigned int y = 0; y < 2; ++y) {
			verts[vertexCount + x * 2  + y] = LineVertex(xMin + (x+1)*pTileSizeX, 0.0f, yMin + y * pTileSizeY * pTileCountY, 1.0f, pColorR, pColorG, pColorB, pAlpha);
			inds[indexCount + x * 2 + y] = vertexOffset + vertexCount + x * 2 + y;
		}
		v += 2;
		ind += 2;
	}
	vertexCount += v;
	indexCount += ind;

	//add vertices and indices to debug geometry arrays
	dMesh dmesh;
	dmesh.offset = indexOffset;
	dmesh.size = indexCount;
	debugMeshLookup.insert(std::pair<std::string, unsigned int>(pName, debugMeshes.size()));
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
	Shader::Data::initVertexAttrib(VAO, 0, 0, 4, GL_FLOAT, offsetof(LineVertex, position));
	Shader::Data::initVertexAttrib(VAO, 0, 1, 4, GL_FLOAT, offsetof(LineVertex, color));
	glVertexArrayElementBuffer(VAO, indexBuffer + 1);

	
	getGLError("initDebugVAOs():");
}

void gl::Debug::init()
{	
	vertexBuffer = Shader::Data::createStorage(vertices.size() * sizeof(LineVertex), &vertices[0], 0);
	indexBuffer = Shader::Data::createStorage(indices.size() * sizeof(unsigned int), &indices[0], 0);
	initDebugVAOs();
	
	getGLError("gl::Debug::init():");
}

void gl::Debug::drawGrid()
{
	glBindVertexArray(VAO);

	Shader::use(lineShaderID);
	glDepthFunc(GL_ALWAYS);
	for (unsigned int m = 0; m < debugMeshes.size(); ++m) {
		if (meshFlags[m]) {
			unsigned int offset = debugMeshes[m].offset;
			unsigned int count = debugMeshes[m].size;
			glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, (void*)(offset*sizeof(unsigned int)));
		}
	}
	glDepthFunc(GL_LESS);

	Shader::unuse();
	glBindVertexArray(0);
	getGLError("drawGrid():");
}

void gl::Debug::initCoordinateSystem(std::string pName)
{
	std::vector<LineVertex> verts;
	std::vector<unsigned int> inds;
	verts.resize(9);
	inds.resize(6*2);
	unsigned int vertexOffset = vertices.size();
	unsigned int indexOffset = indices.size();
	
	
	
	verts[0] = LineVertex(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f);
	verts[1] = LineVertex(-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	verts[2] = LineVertex(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	
	verts[3] = LineVertex(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	verts[4] = LineVertex(0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	verts[5] = LineVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	
	verts[6] = LineVertex(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f);
	verts[7] = LineVertex(0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	verts[8] = LineVertex(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	for (unsigned int i = 0; i < 3; ++i) {
		inds[i * 4] = vertexOffset + i*3;
		inds[i * 4 + 1] = vertexOffset + 1 + i * 3;
		inds[i * 4 + 2] = vertexOffset + i * 3;
		inds[i * 4 + 3] = vertexOffset + 2 + i * 3;
	}

	dMesh dmesh;
	dmesh.offset = indexOffset;
	dmesh.size = 12;
	debugMeshLookup.insert(std::pair<std::string, unsigned int>(pName, debugMeshes.size()));
	debugMeshes.push_back(dmesh);
	meshFlags.push_back(1);
	vertices.insert(vertices.end(), verts.begin(), verts.end());
	indices.insert(indices.end(), inds.begin(), inds.end());
}

void gl::Debug::initDebugShader()
{
	Index lineShader = Shader::newProgram("debugLine", Shader::newModule("gridShader.vert"), Shader::newModule("gridShader.frag"));
	lineShaderID = lineShader.ID;
	Shader::Data::addVertexAttribute(lineShaderID, "position", 0);
	Shader::Data::addVertexAttribute(lineShaderID, "color", 1);
}
