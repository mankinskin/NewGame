#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "Shader.h"
#include "..\..\Global\gl.h"
#include "..\..\Global\glDebug.h"
#include <App\Global\Debug.h>
#include "..\..\Camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm\gtc\type_ptr.hpp>



unsigned int gl::Shader::currentShaderProgram;

std::vector<gl::Shader::Program> gl::Shader::allPrograms;
std::unordered_map<std::string, unsigned int> gl::Shader::shaderProgramLookup;
std::vector<gl::Shader::Module> gl::Shader::allModules;
std::unordered_map<std::string, unsigned int> gl::Shader::moduleLookup;

unsigned int gl::Shader::createModule(std::string pFileName)
{
	unsigned int index = allModules.size();
	moduleLookup.insert(std::pair<std::string, unsigned>(pFileName, index));
	allModules.push_back(Module(pFileName));
	return index;
}

unsigned int gl::Shader::newProgram(std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex)
{
	unsigned int index = createProgram(pProgramName);
	allPrograms[index].type = ProgramType::Basic;
	allPrograms[index].stages[0] = pVertexShaderIndex;
	allPrograms[index].stages[1] = pFragmentShaderIndex;
	allPrograms[index].shaderCount = 2;
	return allPrograms[index].ID;
}

unsigned int gl::Shader::newProgram(std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex, unsigned int pGeometryShaderIndex)
{
	unsigned int index = createProgram(pProgramName);
	allPrograms[index].type = ProgramType::Geometry;
	allPrograms[index].stages[0] = pVertexShaderIndex;
	allPrograms[index].stages[1] = pFragmentShaderIndex;
	allPrograms[index].stages[2] = pGeometryShaderIndex;
	allPrograms[index].shaderCount = 3;
	return allPrograms[index].ID;
}

unsigned int gl::Shader::newProgram(std::string pProgramName, unsigned int pComputeShaderIndex)
{
	unsigned index = createProgram(pProgramName);
	allPrograms[index].type = ProgramType::Compute;
	allPrograms[index].stages[0] = pComputeShaderIndex;
	allPrograms[index].shaderCount = 1;
	return allPrograms[index].ID;
}

unsigned int gl::Shader::createProgram(std::string pProgramName)
{
	Program program;
	program.ID = glCreateProgram();
	program.name = pProgramName;
	shaderProgramLookup.insert(std::pair<std::string, unsigned>(pProgramName, program.ID));
	allPrograms.push_back(program);
	return allPrograms.size()-1;
}

void gl::Shader::use(unsigned int pID)
{
	currentShaderProgram = pID;
	glUseProgram(pID);
}

void gl::Shader::use(std::string pProgramName)
{
	use(shaderProgramLookup.find(pProgramName)->second);
}

void gl::Shader::unuse()
{
	currentShaderProgram = {};
	glUseProgram(0);
}

void gl::Shader::bindUniformBufferToShader(unsigned int pProgram, unsigned int pStorageIndex, std::string pBlockName)
{
	bindUniformBufferToShader(pProgram, VAO::allStorages[pStorageIndex], pBlockName);
}

void gl::Shader::bindUniformBufferToShader(unsigned int pProgram, VAO::Storage& pStorage, std::string pBlockName)
{
	int blockIndex = glGetUniformBlockIndex(pProgram, pBlockName.c_str());
	if (blockIndex < 0) {
		App::Debug::pushError("invalid uniform block name " + pBlockName + "!");
		return;
	}
	glUniformBlockBinding(pProgram, blockIndex, pStorage.binding);
	gl::Debug::getGLError("bindBufferToShader()");
}

void gl::Shader::bindUniformBufferToShader(std::string pProgramName, unsigned int pTargetStorageIndex, std::string pBlockName)
{
	auto it = shaderProgramLookup.find(pProgramName);
	if (it == shaderProgramLookup.end()) {
		App::Debug::pushError("bindUniformBlockBuffer(): Tried to access invalid shader program!");
		return;
	}
	bindUniformBufferToShader(it->second, pTargetStorageIndex, pBlockName);
}

void gl::Shader::addVertexAttribute(unsigned int pProgramID, std::string pAttributeName, unsigned int pAttributeIndex)
{
	//binds a in variable symbol out of a shader to an attribute index
	glBindAttribLocation(pProgramID, pAttributeIndex, pAttributeName.c_str());
}

void gl::Shader::addVertexAttribute(std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex)
{
	auto it = shaderProgramLookup.find(pProgramName.c_str());
	if (it == shaderProgramLookup.end()) {
		App::Debug::pushError("addVertexAttribute():\nCould not find shader Program " + pProgramName + "!\n");
		return;
	}
	addVertexAttribute(it->second, pAttributeName, pAttributeIndex);

}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, int& pValue) {

	glUniform1i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, unsigned int& pValue) {
	
	glUniform1ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, float& pValue) {
	
	glUniform1f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::vec3& pValue) {
	
	glUniform3f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
	
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::vec4& pValue) {

	glUniform4f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::uvec4& pValue) {

	glUniform4ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::uvec3& pValue) {

	glUniform3ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::ivec4& pValue) {
	
	glUniform4i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::ivec3& pValue) {

	glUniform3i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::mat4& pValue, bool pTranspose) {

	glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}

void gl::Shader::setUniform(unsigned int& pProgram, std::string& pUniformName, glm::mat3& pValue, bool pTranspose) {

	glUniformMatrix3fv(glGetUniformLocation(pProgram, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}