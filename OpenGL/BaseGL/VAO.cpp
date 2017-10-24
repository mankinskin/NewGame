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
std::unordered_map<size_t, size_t> gl::VAO::bufferTargetBinds;



size_t gl::VAO::createStorage()
{
    Storage storage;
    size_t ind = allStorages.size();
    glCreateBuffers(1, &storage.ID);
    allStorages.push_back(storage);
    return ind;
}

void gl::VAO::initStorageData(size_t pStorage, size_t pCapacity, const void* pData, size_t pFlags)
{
    Storage& stor = allStorages[pStorage];
    glNamedBufferStorage(stor.ID, pCapacity, pData, pFlags);
    stor.capacity = pCapacity;
    stor.bufferFlags = pFlags;
}

size_t gl::VAO::createStorage(size_t pCapacity, const void* pData, size_t pFlags)
{
    size_t ind = createStorage();
    initStorageData(ind, pCapacity, pData, pFlags);
    Debug::getGLError("createStorage");
    return ind;
}

size_t gl::VAO::createStream(size_t pStorageIndex, size_t pMapFlags)
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

void* gl::VAO::mapStorage(size_t pStorageIndex, size_t pFlags)
{
    return mapStorage(allStorages[pStorageIndex], pFlags);
}

void* gl::VAO::mapStorage(Storage& pStorage, size_t pFlags)
{
    void* p = glMapNamedBufferRange(pStorage.ID, 0, pStorage.capacity, pFlags);
    if (!p) {
	App::Debug::pushError("Failed to map Storage!\n");
    }
    Debug::getGLError("mapStorage():");
    return p;
}


void gl::VAO::streamStorage(size_t pStorageIndex, size_t pUploadSize, void* pData)
{
    streamStorage(allStorages[pStorageIndex], pUploadSize, pData);
}

void gl::VAO::streamStorage(Storage& pStorage, size_t pUploadSize, void* pData)
{

    Stream& stream = allStreams[pStorage.streamIndex];
    stream.updateOffset = stream.updateOffset + stream.lastUpdateSize;
    stream.updateOffset += (stream.alignment - stream.updateOffset%stream.alignment) % stream.alignment;
    size_t updateSize = std::max(pUploadSize, stream.alignment);

    stream.updateOffset = stream.updateOffset * (1 - (stream.updateOffset + updateSize > pStorage.capacity));
    if (pStorage.target == GL_UNIFORM_BUFFER) {
	glBindBufferRange(GL_UNIFORM_BUFFER, pStorage.binding, pStorage.ID, stream.updateOffset, updateSize);
    }
    else if (pStorage.target == GL_ARRAY_BUFFER) {
	glVertexArrayVertexBuffer(pStorage.vaoID, pStorage.binding, pStorage.ID, stream.updateOffset, pStorage.stride);
    }
    stream.lastUpdateSize = updateSize;

    std::memcpy((char*)stream.mappedPtr + stream.updateOffset, pData, pUploadSize);


    Debug::getGLError("updateStreamStorage():");
}

size_t gl::VAO::getStorageID(size_t pStorageIndex)
{
    return allStorages[pStorageIndex].ID;
}

void gl::VAO::setVertexAttrib(size_t pVAO, size_t pBindingIndex, size_t pAttributeIndex, size_t pCount, size_t pType, size_t pOffset, size_t pNormalize) {
    glEnableVertexArrayAttrib(pVAO, pAttributeIndex);
    glVertexArrayAttribBinding(pVAO, pAttributeIndex, pBindingIndex);
    if (pType == GL_FLOAT) {
	glVertexArrayAttribFormat(pVAO, pAttributeIndex, pCount, pType, pNormalize, pOffset);
    }
    else if (pType == GL_INT || pType == GL_UNSIGNED_INT) {
	glVertexArrayAttribIFormat(pVAO, pAttributeIndex, pCount, pType, pOffset);
    }
    else if (pType == GL_DOUBLE) {
	glVertexArrayAttribLFormat(pVAO, pAttributeIndex, pCount, pType, pOffset);
    }
    Debug::getGLError("initVertexAttrib():");
}

void gl::VAO::bindStorage(size_t pTarget, size_t pStorageIndex)
{
    bindStorage(pTarget, allStorages[pStorageIndex]);
    Debug::getGLError("bindStorage():");
}
void gl::VAO::bindStorage(size_t pTarget, Storage& pStorage)
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

void gl::VAO::bindStorageRange(Storage& pStorage, size_t pOffset, size_t pSize)
{
    glBindBufferRange(pStorage.target, pStorage.binding, pStorage.ID, pOffset, pSize);
}
void gl::VAO::bindStorageRange(size_t pStorageIndex, size_t pOffset, size_t pSize)
{
    Storage& stor = allStorages[pStorageIndex];
    glBindBufferRange(stor.target, stor.binding, stor.ID, pOffset, pSize);
}

void gl::VAO::setVertexArrayVertexStorage(size_t pVAO, size_t pBinding, size_t pStorageIndex, size_t pStride)
{
    setVertexArrayVertexStorage(pVAO, pBinding, allStorages[pStorageIndex], pStride);
}

void gl::VAO::setVertexArrayVertexStorage(size_t pVAO, size_t pBinding, Storage & pStorage, size_t pStride)
{
    pStorage.vaoID = pVAO;
    pStorage.target = GL_ARRAY_BUFFER;
    pStorage.binding = pBinding;
    pStorage.stride = pStride;
    //glVertexArrayVertexBuffer(pVAO, pBinding, pStorage.ID, 0, pStride);

}





