#include "..\Global\stdafx.h"
#include "Render.h"
#include "..\BaseGL\VAO.h"
#include "..\BaseGL\Shader.h"
#include "Models.h"
#include <App/World/EntityRegistry.h>
#include "../Global/glDebug.h"
#include "../BaseGL/Framebuffer.h"
unsigned int gl::Models::meshShaderProgram = 0;
unsigned int gl::Models::normalShaderProgram = 0;
unsigned int gl::Models::meshVAO = 0;
unsigned int gl::Models::meshVBO = 0;
unsigned int gl::Models::meshIBO = 0;
unsigned int gl::Models::materialUBO = 0;
unsigned int gl::Models::entityTransformBuffer;
unsigned int gl::Models::transformIndexBuffer;

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
	meshShaderProgram = Shader::newProgram("meshShader", Shader::newModule("meshShader.vert"), Shader::newModule("meshShader.frag")).ID;
	Shader::addVertexAttribute(meshShaderProgram, "pos", 0);
	Shader::addVertexAttribute(meshShaderProgram, "normal", 1);
	Shader::addVertexAttribute(meshShaderProgram, "uv", 2);
}

void gl::Models::fillMeshVAO()
{
	VAO::initStorageData(entityTransformBuffer, sizeof(glm::mat4)*EntityRegistry::MAX_ENTITIES, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::initStorageData(transformIndexBuffer, sizeof(unsigned int)*EntityRegistry::MAX_ENTITIES*Models::MAX_MODELS, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::bindStorage(GL_UNIFORM_BUFFER, entityTransformBuffer);
	VAO::createStream(entityTransformBuffer, GL_MAP_WRITE_BIT);
	VAO::createStream(transformIndexBuffer, GL_MAP_WRITE_BIT);
	
	//-------------------------------
	VAO::initStorageData(meshVBO, sizeof(Vertex)*Models::allVertices.size(), &Models::allVertices[0], 0);
	VAO::initStorageData(meshIBO, sizeof(unsigned int)*Models::allIndices.size(), &Models::allIndices[0], 0);
	glVertexArrayElementBuffer(meshVAO, meshIBO+1);
	glVertexArrayVertexBuffer(meshVAO, 0, meshVBO+1, 0, sizeof(Vertex));
        
	VAO::setVertexAttrib(meshVAO, 0, 0, 3, GL_FLOAT, offsetof(Vertex, pos));
	VAO::setVertexAttrib(meshVAO, 0, 1, 3, GL_FLOAT, offsetof(Vertex, normal));
	VAO::setVertexAttrib(meshVAO, 0, 2, 2, GL_FLOAT, offsetof(Vertex, uv));
        VAO::setVertexArrayVertexStorage(meshVAO, 1, transformIndexBuffer, sizeof(unsigned int));
        VAO::setVertexAttrib(meshVAO, 1, 3, 1, GL_UNSIGNED_INT, 0);
        glVertexArrayBindingDivisor(meshVAO, 1, 1);
}

void gl::Models::storeMaterials()
{
	glNamedBufferStorage(materialUBO, sizeof(Models::Material)*Models::allMaterials.size(), &Models::allMaterials[0], 0);
}

void gl::Models::render()
{
        //for depth test
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl::Texture::gBuffer);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight,
                GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, Texture::gBuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(meshVAO);
        Shader::use(meshShaderProgram);

        for (unsigned int modl = 0; modl < Models::allModels.size(); ++modl) {
                Models::Model& model = Models::allModels[modl];
                if (model.entityCount) {

                        for (unsigned int msh = 0; msh < model.meshCount; ++msh) {
                                Models::Mesh& mesh = Models::allMeshes[model.meshOffset + msh];
                                glActiveTexture(GL_TEXTURE0);//amb
                                glBindTexture(GL_TEXTURE_2D, 0);
                                glActiveTexture(GL_TEXTURE1);//diff
                                glBindTexture(GL_TEXTURE_2D, mesh.texture);
                                glActiveTexture(GL_TEXTURE2);//spec
                                glBindTexture(GL_TEXTURE_2D, 0);


                                glDrawElementsInstanced(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0, model.entityCount);
                        }
                }
        }
        Shader::unuse();
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Debug::getGLError("render(Meshes)");
}

void gl::Models::renderNormals()
{
	glBindVertexArray(meshVAO);
	Shader::use(normalShaderProgram);
	
	for (unsigned int modl = 0; modl < Models::allModels.size(); ++modl) {
		Models::Model& model = Models::allModels[modl];
		if (model.entityCount) {
			
			for (unsigned int msh = 0; msh < model.meshCount; ++msh) {
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
        normalShaderProgram = Shader::newProgram("meshNormalShader", Shader::newModule("meshNormalShader.vert"), Shader::newModule("meshNormalShader.geo"), Shader::newModule("meshNormalShader.frag")).ID;
        Shader::addVertexAttribute(normalShaderProgram, "pos", 0);
        Shader::addVertexAttribute(normalShaderProgram, "normal", 1);
}

void gl::Models::updateBuffers()
{
	if (Models::allInstanceEntities.size()) {
		VAO::streamStorage(entityTransformBuffer, sizeof(glm::mat4)*EntityRegistry::allMatrices.size(), &EntityRegistry::allMatrices[0]);
		VAO::streamStorage(transformIndexBuffer, sizeof(unsigned int)*Models::allInstanceEntities.size(), &Models::allInstanceEntities[0]);
	}
}




