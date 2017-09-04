#include "..\Global\stdafx.h"
#include "Shader_Data.h"
#include "..\Global\glDebug.h"
#include <algorithm>
#include <App\Global\Debug.h>
#include "Shader.h"
#include <glm\gtc\type_ptr.hpp>


std::vector<gl::Shader::Data::Storage> gl::Shader::Data::allStorages;
std::vector<gl::Shader::Data::Stream> gl::Shader::Data::allStreams;

int gl::Shader::Data::UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::Shader::Data::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::Shader::Data::MAX_UNIFORM_BUFFER_BINDINGS = 0;
int gl::Shader::Data::MIN_MAP_BUFFER_ALIGNMENT = 0;
int gl::Shader::Data::STREAM_FLAGS = 0;
std::unordered_map<unsigned int, unsigned int> gl::Shader::Data::bufferTargetBinds;



unsigned int gl::Shader::Data::createStorage(unsigned int pCapacity, const void* pData, unsigned int pFlags)
{
	Storage storage;
	size_t ind = allStorages.size();
	storage.capacity = pCapacity;
	storage.bufferFlags = pFlags;
	glCreateBuffers(1, &storage.ID);
	glNamedBufferStorage(storage.ID, pCapacity, pData, pFlags);
	allStorages.push_back(storage);
	Debug::getGLError("createStorage");
	return ind;
}

unsigned int gl::Shader::Data::createStream(unsigned int pStorageIndex, unsigned int pMapFlags)
{
	Stream stream;
	size_t ind = allStreams.size();
	Storage& storage = allStorages[pStorageIndex];
	storage.streamIndex = ind;
	stream.alignment = UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	//if (storage.target == GL_UNIFORM_BUFFER) {
	//	
	//}
	//else if (storage.target == GL_ARRAY_BUFFER) {
	//
	//}
	stream.mapFlags = pMapFlags | storage.bufferFlags;
	stream.mappedPtr = mapStorage(pStorageIndex, pMapFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	allStreams.push_back(stream);
	Debug::getGLError("gl::createStream():");
	return ind;
}

void* gl::Shader::Data::mapStorage(unsigned int pStorageIndex, unsigned int pFlags)
{
	return mapStorage(allStorages[pStorageIndex], pFlags);
}

void* gl::Shader::Data::mapStorage(Storage& pStorage, unsigned int pFlags)
{
	void* p = glMapNamedBufferRange(pStorage.ID, 0, pStorage.capacity, pFlags);
	if (!p) {
		App::Debug::pushError("Failed to map Storage!\n");
	}
	Debug::getGLError("mapStorage():");
	return p;
}


void gl::Shader::Data::streamStorage(unsigned int pStorageIndex, unsigned int pUploadSize, void* pData)
{
	streamStorage(allStorages[pStorageIndex], pUploadSize, pData);
}

void gl::Shader::Data::streamStorage(Storage& pStorage, unsigned int pUploadSize, void* pData)
{
	
	Stream& stream = allStreams[pStorage.streamIndex];
	stream.updateOffset = stream.updateOffset + stream.lastUpdateSize;
	stream.updateOffset += (stream.alignment - stream.updateOffset%stream.alignment)%stream.alignment;
	unsigned int updateSize = std::max(pUploadSize, stream.alignment);
	
	stream.updateOffset = stream.updateOffset * (1 - (stream.updateOffset + updateSize > pStorage.capacity));
	stream.lastUpdateSize = updateSize;

	std::memcpy((char*)stream.mappedPtr + stream.updateOffset, pData, pUploadSize);

	if (pStorage.target == GL_UNIFORM_BUFFER) {
		glBindBufferRange(pStorage.target, pStorage.binding, pStorage.ID, stream.updateOffset, updateSize);
	}
	else if (pStorage.target == GL_ARRAY_BUFFER) {
		glVertexArrayVertexBuffer(pStorage.vaoID, pStorage.binding, pStorage.ID, stream.updateOffset, pStorage.stride);
	}
	
	
	Debug::getGLError("updateStreamStorage():");
}

unsigned int gl::Shader::Data::getStorageID(unsigned int pStorageIndex)
{
	return allStorages[pStorageIndex].ID;
}

void gl::Shader::Data::initVertexAttrib(unsigned int pVAO, unsigned int pBindingIndex, unsigned int pAttributeIndex, unsigned int pCount, unsigned int pType, unsigned int pOffset, unsigned int pNormalize) {
	glEnableVertexArrayAttrib(pVAO, pAttributeIndex);
	glVertexArrayAttribBinding(pVAO, pAttributeIndex, pBindingIndex);
	if (pType == GL_FLOAT ) {
		glVertexArrayAttribFormat(pVAO, pAttributeIndex, pCount, pType, pNormalize, pOffset);
	}
	else if (pType == GL_INT || pType == GL_UNSIGNED_INT) {
		glVertexArrayAttribIFormat(pVAO, pAttributeIndex, pCount, pType, pOffset);
	}
	else if (pType == GL_DOUBLE ) {
		glVertexArrayAttribLFormat(pVAO, pAttributeIndex, pCount, pType, pOffset);
	}
	Debug::getGLError("initVertexAttrib():");
}

void gl::Shader::Data::bindStorage(unsigned int pTarget, unsigned int pStorageIndex)
{
	bindStorage(pTarget, allStorages[pStorageIndex]);
	Debug::getGLError("bindStorage():");
}
void gl::Shader::Data::bindStorage(unsigned int pTarget, Storage& pStorage)
{
	//binds a storage to an index of a target (like GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER)
	auto& bufferTargetBinding = bufferTargetBinds.find(pTarget);
	if (bufferTargetBinding == bufferTargetBinds.end()) {
		bufferTargetBinding = bufferTargetBinds.insert(std::make_pair(pTarget, 0)).first;
	}
	pStorage.target = pTarget;
	pStorage.binding = bufferTargetBinding->second++;
	
	glBindBufferBase(pStorage.target, pStorage.binding, pStorage.ID);
	
	Debug::getGLError("bindStorage():");
	App::Debug::printErrors();
}

void gl::Shader::Data::bindVertexArrayVertexStorage(unsigned int pStorageIndex, unsigned int pVAO, unsigned int pTarget, unsigned int pBinding, unsigned int pStride)
{
	bindVertexArrayVertexStorage(allStorages[pStorageIndex], pVAO, pTarget, pBinding, pStride);
}

void gl::Shader::Data::bindVertexArrayVertexStorage(Storage & pStorage, unsigned int pVAO, unsigned int pTarget, unsigned int pBinding, unsigned int pStride)
{
	pStorage.vaoID = pVAO;
	pStorage.target = pTarget;
	pStorage.binding = pBinding;
	pStorage.stride = pStride;
	if (pTarget == GL_ARRAY_BUFFER) {
		glVertexArrayVertexBuffer(pVAO, pBinding, pStorage.ID, 0, pStride);
	}
}


void gl::Shader::Data::addVertexAttribute(unsigned int pProgramID, std::string pAttributeName, unsigned int pAttributeIndex)
{
	//binds a in variable symbol out of a shader to an attribute index
	glBindAttribLocation(pProgramID, pAttributeIndex, pAttributeName.c_str());
}

void gl::Shader::Data::addVertexAttribute(std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex)
{
	auto it = shaderProgramLookup.find(pProgramName.c_str());
	if (it == shaderProgramLookup.end()) {
		App::Debug::pushError("addVertexAttribute():\nCould not find shader Program " + pProgramName + "!\n");
		return;
	}
	addVertexAttribute(it->second.ID, pAttributeName, pAttributeIndex);

}

void gl::Shader::Data::bindBufferToUniformBlock(unsigned int pProgram, unsigned int pStorageIndex, std::string pBlockName)
{
	Storage& storage = allStorages[pStorageIndex];
	bindBufferToUniformBlock(pProgram, storage, pBlockName);
	allStorages[pStorageIndex] = storage;
}

void gl::Shader::Data::bindBufferToUniformBlock(unsigned int pProgram, Storage& pStorage, std::string pBlockName)
{
	int blockIndex = glGetUniformBlockIndex(pProgram, pBlockName.c_str());
	if (blockIndex < 0) {
		App::Debug::pushError("invalid uniform block name " + pBlockName + "!");
		return;
	}
	glUniformBlockBinding(pProgram, blockIndex, pStorage.binding);
	gl::Debug::getGLError("bindUniformBlockBuffer()");
}

void gl::Shader::Data::bindBufferToUniformBlock(std::string pProgramName, unsigned int pTargetStorageIndex, std::string pBlockName)
{
	auto it = shaderProgramLookup.find(pProgramName);
	if (it == shaderProgramLookup.end()) {
		App::Debug::pushError("bindUniformBlockBuffer(): Tried to access invalid shader program!");
		return;
	}
	bindBufferToUniformBlock(it->second.ID, pTargetStorageIndex, pBlockName);
}


void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, int& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform1i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
	gl::Debug::getGLError("glUniform1i()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, unsigned int& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform1ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
	gl::Debug::getGLError("glUniform1ui()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, float& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform1f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
	gl::Debug::getGLError("glUniform1f()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::vec3& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform3f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
	gl::Debug::getGLError("glUniform3f()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::vec4& pValue) {
	if (currentShaderProgram.ID < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}

	glUniform4f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
	gl::Debug::getGLError("glUniform3f()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::uvec4& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform4ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
	gl::Debug::getGLError("glUniform4ui()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::uvec3& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform3ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
	gl::Debug::getGLError("glUniform3ui()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::ivec4& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform4i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
	gl::Debug::getGLError("setUniform4i()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::ivec3& pValue) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniform3i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
	gl::Debug::getGLError("setUniform3i()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::mat4& pValue, bool pTranspose) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	unsigned int loc = glGetUniformLocation(currentShaderProgram.ID, pUniformName.c_str());
	glUniformMatrix4fv(loc, 1, pTranspose, glm::value_ptr(pValue));
	gl::Debug::getGLError("setUniformMatrix4fv()");
}

void gl::Shader::Data::setUniform(unsigned int pProgram, std::string pUniformName, glm::mat3& pValue, bool pTranspose) {
	if (pProgram < 1) {
		App::Debug::pushError("Uniform variable binding failed: No ShaderProgram in use", App::Debug::Error::Fatal);
		return;
	}
	glUniformMatrix3fv(glGetUniformLocation(pProgram, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
	gl::Debug::getGLError("setUniformMatrix3f()");
}
