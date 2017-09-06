#include "Global\stdafx.h"
#include "Render.h"
#include "BaseGL\VAO.h"
#include "BaseGL/Shader.h"


unsigned int gl::Render::meshShaderProgram = 0;
unsigned int gl::Render::meshVAO = 0;
unsigned int gl::Render::meshVBO = 0;
unsigned int gl::Render::meshIBO = 0;
unsigned int gl::Render::materialUBO = 0;
std::vector<gl::Render::Material> gl::Render::allMaterials;
std::vector<gl::Render::Mesh> gl::Render::allMeshes;
std::vector<unsigned int> gl::Render::allIndices;
std::vector<gl::Render::Vertex> gl::Render::allVertices;


void gl::Render::initMeshVAO()
{
	glCreateVertexArrays(1, &meshVAO);
	glCreateBuffers(2, &meshVBO);

}

void gl::Render::initMeshShader()
{
	meshShaderProgram = Shader::newProgram("meshShader", Shader::newModule("meshShader.vert"), Shader::newModule("meshShader.frag")).ID;
	Shader::addVertexAttribute(meshShaderProgram, "pos", 0);
	Shader::addVertexAttribute(meshShaderProgram, "normal", 1);
	Shader::addVertexAttribute(meshShaderProgram, "uv", 2);
}

void gl::Render::fillMeshVAO()
{
	//hardcoded cube
	std::vector<Vertex> verts = {
		Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f, 1.0f,  -1.0,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
	};
	std::vector<unsigned> inds = {
		0, 3, 1, 1, 3, 2, //downside
		0, 4, 3, 3, 4, 7, //left side
		0, 5, 4, 0, 1, 5, //front
		4, 5, 6, 4, 6, 7, //upside
		7, 6, 3, 3, 6, 2, //back
		2, 6, 1, 1, 6, 5  //right
	};
	allMeshes.push_back(Mesh(newGeometry(verts, inds)));

	//-------------------------------
	unsigned vbo = VAO::createStorage(sizeof(Vertex)*allVertices.size(), &allVertices[0], 0);
	meshVBO = VAO::getStorageID(vbo);
	unsigned ibo = VAO::createStorage(sizeof(unsigned int)*allIndices.size(), &allIndices[0], 0);
	meshIBO = VAO::getStorageID(ibo);
	glVertexArrayElementBuffer(meshVAO, meshIBO);
	VAO::bindVertexArrayVertexStorage(meshVAO, 0, vbo, sizeof(Vertex));
	VAO::initVertexAttrib(meshVAO, 0, 0, 3, GL_FLOAT, offsetof(Vertex, pos));
	VAO::initVertexAttrib(meshVAO, 0, 1, 3, GL_FLOAT, offsetof(Vertex, normal));
	VAO::initVertexAttrib(meshVAO, 0, 2, 2, GL_FLOAT, offsetof(Vertex, uv));
}

void gl::Render::storeMaterials()
{
	glNamedBufferStorage(materialUBO, sizeof(Material)*allMaterials.size(), &allMaterials[0], 0);
}

void gl::Render::render()
{
	Shader::use(meshShaderProgram);
	glBindVertexArray(meshVAO);

	glDrawElements(GL_TRIANGLES, allMeshes[0].geometry.indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	Shader::unuse();
}

gl::Render::Geometry gl::Render::newGeometry(std::vector<Vertex> pVerts, std::vector<unsigned int> pIndices)
{
	Geometry geom(allIndices.size(), pIndices.size());
	allIndices.insert(allIndices.end(), pIndices.begin(), pIndices.end());
	allVertices.insert(allVertices.end(), pVerts.begin(), pVerts.end());
	return geom;
}

