#include "..\Global\stdafx.h"
#include "VAO.h"
#include "..\Global\glDebug.h"
#include <algorithm>
#include <App\Global\Debug.h>
#include <glm\gtc\type_ptr.hpp>


std::vector<gl::VAO::Storage> gl::VAO::allStorages;
std::vector<gl::VAO::Stream> gl::VAO::allStreams;

int gl::VAO::UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::VAO::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::VAO::MAX_UNIFORM_BUFFER_BINDINGS = 0;
int gl::VAO::MIN_MAP_BUFFER_ALIGNMENT = 0;
int gl::VAO::STREAM_FLAGS = 0;
std::unordered_map<unsigned int, unsigned int> gl::VAO::bufferTargetBinds;



unsigned int gl::VAO::createStorage()
{
	Storage storage;
	unsigned ind = allStorages.size();
	glCreateBuffers(1, &storage.ID);
	allStorages.push_back(storage);
	return ind;
}

void gl::VAO::initStorageData(unsigned int pStorage, unsigned int pCapacity, const void* pData, unsigned int pFlags)
{
	Storage& stor = allStorages[pStorage];
	glNamedBufferStorage(stor.ID, pCapacity, pData, pFlags);
	stor.capacity = pCapacity;
	stor.bufferFlags = pFlags;
}

unsigned int gl::VAO::createStorage(unsigned int pCapacity, const void* pData, unsigned int pFlags)
{
	unsigned ind = createStorage();
	initStorageData(ind, pCapacity, pData, pFlags);
	Debug::getGLError("createStorage");
	return ind;
}

unsigned int gl::VAO::createStream(unsigned int pStorageIndex, unsigned int pMapFlags)
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
	stream.mapFlags = pMapFlags | storage.bufferFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	stream.mappedPtr = mapStorage(pStorageIndex, pMapFlags);
	allStreams.push_back(stream);
	Debug::getGLError("gl::createStream():");
	return ind;
}

void* gl::VAO::mapStorage(unsigned int pStorageIndex, unsigned int pFlags)
{
	return mapStorage(allStorages[pStorageIndex], pFlags);
}

void* gl::VAO::mapStorage(Storage& pStorage, unsigned int pFlags)
{
	void* p = glMapNamedBufferRange(pStorage.ID, 0, pStorage.capacity, pFlags);
	if (!p) {
		App::Debug::pushError("Failed to map Storage!\n");
	}
	Debug::getGLError("mapStorage():");
	return p;
}


void gl::VAO::streamStorage(unsigned int pStorageIndex, unsigned int pUploadSize, void* pData)
{
	streamStorage(allStorages[pStorageIndex], pUploadSize, pData);
}

void gl::VAO::streamStorage(Storage& pStorage, unsigned int pUploadSize, void* pData)
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

unsigned int gl::VAO::getStorageID(unsigned int pStorageIndex)
{
	return allStorages[pStorageIndex].ID;
}

void gl::VAO::initVertexAttrib(unsigned int pVAO, unsigned int pBindingIndex, unsigned int pAttributeIndex, unsigned int pCount, unsigned int pType, unsigned int pOffset, unsigned int pNormalize) {
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

void gl::VAO::bindStorage(unsigned int pTarget, unsigned int pStorageIndex)
{
	bindStorage(pTarget, allStorages[pStorageIndex]);
	Debug::getGLError("bindStorage():");
}
void gl::VAO::bindStorage(unsigned int pTarget, Storage& pStorage)
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

void gl::VAO::bindStorageRange(Storage& pStorage, unsigned int pOffset, unsigned int pSize)
{
	glBindBufferRange(pStorage.target, pStorage.binding, pStorage.ID, pOffset, pSize);
}
void gl::VAO::bindStorageRange(unsigned int pStorageIndex, unsigned int pOffset, unsigned int pSize)
{
	Storage& stor = allStorages[pStorageIndex];
	glBindBufferRange(stor.target, stor.binding, stor.ID, pOffset, pSize);
}

void gl::VAO::bindVertexArrayVertexStorage(unsigned int pVAO, unsigned int pBinding, unsigned int pStorageIndex, unsigned int pStride)
{
	bindVertexArrayVertexStorage(pVAO, pBinding, allStorages[pStorageIndex], pStride);
}

void gl::VAO::bindVertexArrayVertexStorage(unsigned int pVAO, unsigned int pBinding, Storage & pStorage, unsigned int pStride)
{
	pStorage.vaoID = pVAO;
	pStorage.target = GL_ARRAY_BUFFER;
	pStorage.binding = pBinding;
	pStorage.stride = pStride;
	glVertexArrayVertexBuffer(pVAO, pBinding, pStorage.ID, 0, pStride);
	
}





