#include "..\Global\stdafx.h"
#include "Render.h"
#include "..\BaseGL\VAO.h"
#include "..\BaseGL\Shader.h"
#include "Models.h"
#include <App/World/EntityRegistry.h>
unsigned int gl::Render::meshShaderProgram = 0;
unsigned int gl::Render::meshVAO = 0;
unsigned int gl::Render::meshVBO = 0;
unsigned int gl::Render::meshIBO = 0;
unsigned int gl::Render::materialUBO = 0;
unsigned int gl::Render::entityTransformBuffer;
unsigned int gl::Render::transformIndexBuffer;



void gl::Render::initMeshVAO()
{
	glCreateVertexArrays(1, &meshVAO);
	meshVBO = VAO::createStorage() + 1;
	meshIBO = VAO::createStorage() + 1;
	entityTransformBuffer = VAO::createStorage();
	transformIndexBuffer = VAO::createStorage();
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
	VAO::initStorageData(entityTransformBuffer, sizeof(glm::mat4)*EntityRegistry::MAX_ENTITIES, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::initStorageData(transformIndexBuffer, sizeof(unsigned int)*EntityRegistry::MAX_ENTITIES*Models::MAX_MODELS, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::bindStorage(GL_UNIFORM_BUFFER, entityTransformBuffer);
	VAO::bindStorage(GL_UNIFORM_BUFFER, transformIndexBuffer);
	VAO::createStream(entityTransformBuffer, GL_MAP_WRITE_BIT);
	VAO::createStream(transformIndexBuffer, GL_MAP_WRITE_BIT);
	
	//-------------------------------
	VAO::initStorageData(meshVBO - 1, sizeof(Vertex)*Models::allVertices.size(), &Models::allVertices[0], 0);
	VAO::initStorageData(meshIBO - 1, sizeof(unsigned int)*Models::allIndices.size(), &Models::allIndices[0], 0);
	glVertexArrayElementBuffer(meshVAO, meshIBO);
	glVertexArrayVertexBuffer(meshVAO, 0, meshVBO, 0, sizeof(Vertex));
	VAO::setVertexAttrib(meshVAO, 0, 0, 3, GL_FLOAT, offsetof(Vertex, pos));
	VAO::setVertexAttrib(meshVAO, 0, 1, 3, GL_FLOAT, offsetof(Vertex, normal));
	VAO::setVertexAttrib(meshVAO, 0, 2, 2, GL_FLOAT, offsetof(Vertex, uv));
}

void gl::Render::storeMaterials()
{
	glNamedBufferStorage(materialUBO, sizeof(Models::Material)*Models::allMaterials.size(), &Models::allMaterials[0], 0);
}

void gl::Render::render()
{
	glBindVertexArray(meshVAO);
	Shader::use(meshShaderProgram);
	for (unsigned int modl = 0; modl < Models::allModels.size(); ++modl) {
		Models::Model& model = Models::allModels[modl];
		VAO::bindStorageRange(transformIndexBuffer, model.entityOffset, model.entityCount);
		for (unsigned int msh = 0; msh < model.meshCount; ++msh) {
			glDrawElementsInstanced(GL_TRIANGLES, Models::allMeshes[model.meshOffset + msh].geometry.indexCount, GL_UNSIGNED_INT, 0, model.entityCount);
		}
	}
	Shader::unuse();
	glBindVertexArray(0);
}

void gl::Render::updateBuffers()
{
	VAO::streamStorage(entityTransformBuffer, sizeof(glm::mat4)*EntityRegistry::allMatrices.size(), &EntityRegistry::allMatrices[0]);
	VAO::streamStorage(transformIndexBuffer, sizeof(unsigned int)*Models::allInstanceEntities.size(), &Models::allInstanceEntities[0]);

}


