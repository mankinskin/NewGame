#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Render.h"
#include "..\BaseGL\VAO.h"
#include "../BaseGL/Shader/Shader.h"
#include "Models.h"
#include <App/World/EntityRegistry.h>
#include "../Global/glDebug.h"
#include "../BaseGL/Framebuffer.h"


void gl::Models::initMeshVAO()
{
    glCreateVertexArrays(1, &meshVAO);
    meshVBO = VAO::createStorage();
    meshIBO = VAO::createStorage();
    entityTransformBuffer = VAO::createStorage();
    transformIndexBuffer = VAO::createStorage();
}

void gl::Models::initMeshShader()
{
    meshShaderProgram = Shader::newProgram("meshShader", Shader::createModule("meshShader.vert"), Shader::createModule("meshShader.frag"));
    Shader::addVertexAttribute(meshShaderProgram, "pos", 0);
    Shader::addVertexAttribute(meshShaderProgram, "normal", 1);
    Shader::addVertexAttribute(meshShaderProgram, "uv", 2);
}

void gl::Models::fillMeshVAO()
{
    VAO::initStorageData(entityTransformBuffer, sizeof(glm::mat4)*EntityRegistry::MAX_ENTITIES, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
    VAO::initStorageData(transformIndexBuffer, sizeof(size_t)*EntityRegistry::MAX_ENTITIES*Models::MAX_MODELS, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
    VAO::bindStorage(GL_UNIFORM_BUFFER, entityTransformBuffer);
    VAO::createStream(entityTransformBuffer, GL_MAP_WRITE_BIT);
    VAO::createStream(transformIndexBuffer, GL_MAP_WRITE_BIT);

    //-------------------------------
    VAO::initStorageData(meshVBO, sizeof(Vertex)*Models::allVertices.size(), &Models::allVertices[0], 0);
    VAO::initStorageData(meshIBO, sizeof(size_t)*Models::allIndices.size(), &Models::allIndices[0], 0);
    glVertexArrayElementBuffer(meshVAO, meshIBO + 1);
    glVertexArrayVertexBuffer(meshVAO, 0, meshVBO + 1, 0, sizeof(Vertex));

    VAO::setVertexAttrib(meshVAO, 0, 0, 3, GL_FLOAT, offsetof(Vertex, pos));
    VAO::setVertexAttrib(meshVAO, 0, 1, 3, GL_FLOAT, offsetof(Vertex, normal));
    VAO::setVertexAttrib(meshVAO, 0, 2, 2, GL_FLOAT, offsetof(Vertex, uv));
    VAO::setVertexArrayVertexStorage(meshVAO, 1, transformIndexBuffer, sizeof(size_t));
    VAO::setVertexAttrib(meshVAO, 1, 3, 1, GL_UNSIGNED_INT, 0);
    glVertexArrayBindingDivisor(meshVAO, 1, 1);
}

void gl::Models::storeMaterials()
{
    glNamedBufferStorage(materialUBO, sizeof(Models::Material)*Models::allMaterials.size(), &Models::allMaterials[0], 0);
}

void gl::Models::render()
{


    glBindVertexArray(meshVAO);
    Shader::use(meshShaderProgram);

    for (size_t modl = 0; modl < Models::allModels.size(); ++modl) {
	Models::Model& model = Models::allModels[modl];
	if (model.entityCount) {

	    for (size_t msh = 0; msh < model.meshCount; ++msh) {
		Models::Mesh& mesh = Models::allMeshes[model.meshOffset + msh];
		glActiveTexture(GL_TEXTURE0);//amb
		glBindTexture(GL_TEXTURE_2D, mesh.texture);
		glActiveTexture(GL_TEXTURE1);//diff
		glBindTexture(GL_TEXTURE_2D, mesh.texture);
		glActiveTexture(GL_TEXTURE2);//spec
		glBindTexture(GL_TEXTURE_2D, mesh.texture);


		glDrawElementsInstanced(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0, model.entityCount);
	    }
	}
    }
    Shader::unuse();
    glBindVertexArray(0);

    Debug::getGLError("render(Meshes)");
}

void gl::Models::renderNormals()
{
    glBindVertexArray(meshVAO);
    Shader::use(normalShaderProgram);

    for (size_t modl = 0; modl < Models::allModels.size(); ++modl) {
	Models::Model& model = Models::allModels[modl];
	if (model.entityCount) {

	    for (size_t msh = 0; msh < model.meshCount; ++msh) {
		Models::Mesh& mesh = Models::allMeshes[model.meshOffset + msh];

		glDrawArraysInstanced(GL_POINTS, mesh.vertexOffset, mesh.vertexCount, model.entityCount);
	    }
	}
    }
    Shader::unuse();
    glBindVertexArray(0);
    Debug::getGLError("render(Mesh Normals)");
}



void gl::Models::initNormalShader()
{
    normalShaderProgram = Shader::newProgram("meshNormalShader", Shader::createModule("meshNormalShader.vert"), Shader::createModule("meshNormalShader.geo"), Shader::createModule("meshNormalShader.frag"));
    Shader::addVertexAttribute(normalShaderProgram, "pos", 0);
    Shader::addVertexAttribute(normalShaderProgram, "normal", 1);
}

void gl::Models::updateBuffers()
{
    if (Models::allInstanceEntities.size()) {
	VAO::streamStorage(entityTransformBuffer, sizeof(glm::mat4)*EntityRegistry::allMatrices.size(), &EntityRegistry::allMatrices[0]);
	VAO::streamStorage(transformIndexBuffer, sizeof(size_t)*Models::allInstanceEntities.size(), &Models::allInstanceEntities[0]);
    }
}




