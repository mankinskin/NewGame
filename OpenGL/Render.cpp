#include "Global\stdafx.h"
#include "Render.h"
#include <App\Entity.h>

unsigned int gl::Render::meshVAO;
unsigned int gl::Render::meshVBO;
unsigned int gl::Render::meshIBO;
unsigned int gl::Render::materialUBO;
std::vector<gl::Render::Mesh> gl::Render::allMeshes;
std::vector<unsigned int> gl::Render::allIndices;
std::vector<gl::Render::Vertex> gl::Render::allVertices;

void gl::Render::initMeshVAO()
{
	glCreateVertexArrays(1, &meshVAO);
	glCreateBuffers(2, &meshVBO);

}

void gl::Render::fillMeshVAO()
{
	glNamedBufferStorage(meshVBO, sizeof(Vertex)*allVertices.size(), &allVertices[0], 0);
	glNamedBufferStorage(meshIBO, sizeof(unsigned)*allIndices.size(), &allIndices[0], 0);
	glVertexArrayVertexBuffer(meshVAO, 0, meshVBO, 0, sizeof(Vertex));//mesh verts
	glVertexArrayElementBuffer(meshVAO,  meshIBO);//mesh inds
	
}

void gl::Render::storeMaterials()
{
	glNamedBufferStorage(materialUBO, sizeof(Material)*allMaterials.size(), &allMaterials[0], 0);
}

unsigned int gl::Render::newMesh(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices)
{
	
	Mesh mesh(allIndices.size(), pIndices.size());
	allIndices.insert(allIndices.end(), pIndices.begin(), pIndices.end());
	allVertices.insert(allVertices.end(), pVerts.begin(), pVerts.end());
	allMeshes.push_back(mesh);
	return allMeshes.size()-1;
}

