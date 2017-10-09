#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Lights.h"
#include "../BaseGL/VAO.h"
#include "../BaseGL/Shader/Shader.h"
#include "../Global/gl.h"
#include "../BaseGL/Framebuffer.h"
#include "../Global/glDebug.h"
#include "../Models/Render.h"
#include "../BaseGL/Texture.h"

std::vector<glm::vec4> gl::Lighting::allLightData;
std::vector<gl::Lighting::LightIndexRange> gl::Lighting::allLightIndexRanges;
unsigned int gl::Lighting::lightVAO = 0;
unsigned int gl::Lighting::lightIndexVBO = 0;
unsigned int gl::Lighting::lightDataUBO = 0;
unsigned int gl::Lighting::lightShaderProgram = 0;
unsigned int gl::Lighting::MAX_LIGHT_COUNT = 100;

void gl::Lighting::initLightVAO(){
        
        lightIndexVBO = VAO::createStorage(MAX_LIGHT_COUNT*sizeof(LightIndexRange), 0, VAO::STREAM_FLAGS | GL_MAP_WRITE_BIT);
        VAO::createStream(lightIndexVBO, GL_MAP_WRITE_BIT);
       
        glCreateVertexArrays(1, &lightVAO);
	glVertexArrayVertexBuffer(lightVAO, 0, gl::quadVBO + 1, 0, sizeof(float)*2);
        glVertexArrayElementBuffer(lightVAO, gl::quadEBO + 1);
        VAO::setVertexArrayVertexStorage(lightVAO, 1, lightIndexVBO, sizeof(unsigned int)*2);
        glVertexArrayBindingDivisor(lightVAO, 1, 1);
	VAO::setVertexAttrib(lightVAO, 0, 0, 2, GL_FLOAT, 0);
	VAO::setVertexAttrib(lightVAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	
}

void gl::Lighting::initLightDataBuffer(){

        lightDataUBO = VAO::createStorage(MAX_LIGHT_COUNT*sizeof(glm::vec4)*3, 0, VAO::STREAM_FLAGS | GL_MAP_WRITE_BIT); 
        VAO::createStream(lightDataUBO, GL_MAP_WRITE_BIT);
	VAO::bindStorage(GL_UNIFORM_BUFFER, lightDataUBO);
}

void gl::Lighting::updateLightDataBuffer(){
        if(allLightData.size()){
                VAO::streamStorage(lightDataUBO, allLightData.size()*sizeof(glm::vec4), &allLightData[0]);
        }
}

void gl::Lighting::updateLightIndexRangeBuffer(){
	if(allLightIndexRanges.size()){
		VAO::streamStorage(lightIndexVBO, allLightIndexRanges.size()*sizeof(LightIndexRange), &allLightIndexRanges[0]);
	}
}
void gl::Lighting::initLightShader(){
        lightShaderProgram =  Shader::newProgram("lightShaderProgram", Shader::createModule("lightShaderProgram.vert"), Shader::createModule("lightShaderProgram.frag"));
        Shader::addVertexAttribute(lightShaderProgram, "corner_pos", 0);
        Shader::addVertexAttribute(lightShaderProgram, "index_range", 1);
}

unsigned int gl::Lighting::createLight(glm::vec4& pPos, glm::vec4& pColor){
        allLightIndexRanges.push_back(LightIndexRange(allLightData.size(), 2));
        allLightData.push_back(pPos);
        allLightData.push_back(pColor);
        return allLightIndexRanges.size()-1;
}

unsigned int gl::Lighting::createLight(glm::vec4& pPos, glm::vec4& pColor, glm::vec4& pFrustum){
        allLightIndexRanges.push_back(LightIndexRange(allLightData.size(), 3));
        allLightData.push_back(pPos);
        allLightData.push_back(pColor);
        allLightData.push_back(pFrustum);
        return allLightIndexRanges.size()-1;
}
void gl::Lighting::reserveLightSpace(unsigned int pCount){
	allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
	allLightData.reserve(allLightData.capacity() + pCount*3);
}
void gl::Lighting::reservePointLightSpace(unsigned int pCount){
	allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
	allLightData.reserve(allLightData.capacity() + pCount*2);	
}
void gl::Lighting::renderLights()
{
        glDepthMask(0);
        
        
        glBlendFunc(GL_ONE, GL_ONE);
	glBindVertexArray(lightVAO);
	Shader::use(lightShaderProgram);
        glBlendFunc(GL_ONE, GL_ONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture::gAmbientTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture::gDiffuseTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Texture::gSpecularTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Texture::gPosTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Texture::gNormalTexture);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allLightIndexRanges.size());
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Shader::unuse();
	glBindVertexArray(0);
        glDepthMask(1);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, Texture::gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, screenWidth * resolution, screenHeight * resolution, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        Debug::getGLError("renderLights()4");
}
void gl::Lighting::setupLightShader()
{
	Shader::bindUniformBufferToShader(lightShaderProgram, lightDataUBO, "LightDataBuffer");
	Shader::bindUniformBufferToShader(lightShaderProgram, generalUniformBuffer, "GeneralUniformBuffer");
}

void gl::Lighting::setLightPos(unsigned int pLightIndex, glm::vec3& pPos){
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset], &pPos, sizeof(float)*3);
}
void gl::Lighting::setLightPos(unsigned int pLightIndex, glm::vec4& pPos){
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset], &pPos, sizeof(float)*4);
}
void gl::Lighting::setLightColor(unsigned int pLightIndex, glm::vec3& pColor){
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset+1], &pColor, sizeof(float)*3);
}
void gl::Lighting::setLightColor(unsigned int pLightIndex, glm::vec4& pColor){
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset+1], &pColor, sizeof(float)*4);
}
