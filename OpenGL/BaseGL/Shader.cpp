#include "..\Global\stdafx.h"
#include "Shader.h"
#include "..\Global\gl.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>

#include "..\Camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm\gtc\type_ptr.hpp>



unsigned int gl::Shader::PROGRAM_COUNT;
unsigned int gl::Shader::MODULE_COUNT;
gl::Index gl::Shader::currentShaderProgram;

std::vector<gl::Shader::Program> gl::Shader::allPrograms;
std::unordered_map<std::string, gl::Index> gl::Shader::shaderProgramLookup;
std::vector<gl::Shader::Module> gl::Shader::allModules;
std::unordered_map<std::string, gl::Index> gl::Shader::moduleLookup;



gl::Index gl::Shader::newModule(string pFileName)
{
	Index index;
	index.index = MODULE_COUNT++;
	moduleLookup.insert(std::pair<string, Index>(pFileName, index));
	allModules.push_back(Module(pFileName));
	return index;
}

gl::Index gl::Shader::newProgram(string pProgramName, gl::Index pVertexShaderID, gl::Index pFragmentShaderID)
{
	Index index = addProgram(pProgramName);
	allPrograms[index.index].type = ProgramType::Basic;
	allPrograms[index.index].stages[0] = pVertexShaderID;
	allPrograms[index.index].stages[1] = pFragmentShaderID;
	allPrograms[index.index].shaderCount = 2;
	return index;
}

gl::Index gl::Shader::newProgram(string pProgramName, gl::Index pVertexShaderID, gl::Index pFragmentShaderID, gl::Index pGeometryShaderID)
{
	Index index = addProgram(pProgramName);
	allPrograms[index.index].type = ProgramType::Geometry;
	allPrograms[index.index].stages[0] = pVertexShaderID;
	allPrograms[index.index].stages[1] = pFragmentShaderID;
	allPrograms[index.index].stages[2] = pGeometryShaderID;
	allPrograms[index.index].shaderCount = 3;
	return index;
}

gl::Index gl::Shader::newProgram(string pProgramName, gl::Index pComputeShaderID)
{
	Index index = addProgram(pProgramName);
	allPrograms[index.index].type = ProgramType::Compute;
	allPrograms[index.index].stages[0] = pComputeShaderID;
	allPrograms[index.index].shaderCount = 1;
	return index;
}

gl::Index gl::Shader::addProgram(string pProgramName)
{
	Program program;
	program.ID = glCreateProgram();
	program.name = pProgramName;
	Index index;
	index.index = PROGRAM_COUNT++;
	index.ID = program.ID;
	shaderProgramLookup.insert(std::pair<string, Index>(pProgramName, index));
	allPrograms.push_back(program);
	
	return index;
}

void gl::Shader::use(unsigned int pID)
{
	currentShaderProgram.ID = pID;
	glUseProgram(pID);
}

void gl::Shader::use(string pProgramName)
{
	use(shaderProgramLookup.find(pProgramName)->second.ID);
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
	bindUniformBufferToShader(it->second.ID, pTargetStorageIndex, pBlockName);
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
	addVertexAttribute(it->second.ID, pAttributeName, pAttributeIndex);

}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, int& pValue) {

	glUniform1i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, unsigned int& pValue) {
	
	glUniform1ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, float& pValue) {
	
	glUniform1f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::vec3& pValue) {
	
	glUniform3f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
	
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::vec4& pValue) {

	glUniform4f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::uvec4& pValue) {

	glUniform4ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::uvec3& pValue) {

	glUniform3ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::ivec4& pValue) {
	
	glUniform4i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::ivec3& pValue) {

	glUniform3i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::mat4& pValue, bool pTranspose) {

	glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram.ID, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}

void gl::Shader::setUniform(unsigned int pProgram, std::string pUniformName, glm::mat3& pValue, bool pTranspose) {

	glUniformMatrix3fv(glGetUniformLocation(pProgram, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}