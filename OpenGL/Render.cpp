#include "Global\stdafx.h"
#include "Render.h"
#include "BaseGL\VAO.h"

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

gl::Render::Geometry gl::Render::newGeometry(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices)
{
	Geometry geom(allIndices.size(), pIndices.size());
	allIndices.insert(allIndices.end(), pIndices.begin(), pIndices.end());
	allVertices.insert(allVertices.end(), pVerts.begin(), pVerts.end());
	return geom;
}

