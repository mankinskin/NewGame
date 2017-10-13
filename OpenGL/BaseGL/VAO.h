#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace gl {
    namespace VAO {

	struct Storage {
	    Storage() {}
	    size_t ID = 0;
	    size_t capacity = 0;
	    size_t bufferFlags = 0;
	    size_t streamIndex = 0;
	    size_t target = 0;
	    size_t binding = 0;

	    size_t vaoID = 0;
	    size_t stride = 0;
	};

	size_t createStorage();
	void initStorageData(size_t pStorage, size_t pCapacity, const void * pData, size_t pFlags);
	size_t createStorage(size_t pCapacity, const void* pData, size_t pFlags);

	struct Stream {
	    Stream() : mapFlags(0), mappedPtr(nullptr), alignment(16), updateOffset(0), lastUpdateSize(0) {}
	    size_t mapFlags;
	    void* mappedPtr;
	    size_t alignment;
	    size_t updateOffset;
	    size_t lastUpdateSize;
	};

	size_t createStream(size_t pStorageIndex, size_t pMapFlags);


	size_t getStorageID(size_t pStorageIndex);
	void streamStorage(size_t pStorageIndex, size_t pUploadSize, void * pData);
	void streamStorage(Storage& pStorage, size_t pUploadSize, void * pData);
	void * mapStorage(size_t pStorageIndex, size_t pFlags);
	void * mapStorage(Storage& pStorage, size_t pFlags);
	void bindStorage(size_t pTarget, size_t pStorageIndex);
	void bindStorage(size_t pTarget, Storage& pStorage);
	void bindStorageRange(Storage& pStorage, size_t pOffset, size_t pSize);
	void bindStorageRange(size_t pStorageIndex, size_t pOffset, size_t pSize);
	void setVertexArrayVertexStorage(size_t pVAO, size_t pBinding, size_t pStorageIndex, size_t pStride);
	void setVertexArrayVertexStorage(size_t pVAO, size_t pBinding, Storage& pStorage, size_t pStride);
	void setVertexAttrib(size_t pVAO, size_t pBindingIndex, size_t pAttributeIndex, size_t pCount, size_t pType, size_t pOffset, size_t pNormalize = false);

	extern int MIN_MAP_BUFFER_ALIGNMENT;
	extern int STREAM_FLAGS;
	extern int UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	extern int SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
	extern int MAX_UNIFORM_BUFFER_BINDINGS;
	extern std::unordered_map<size_t, size_t> bufferTargetBinds;
	extern std::vector<Storage> allStorages;
	extern std::vector<Stream> allStreams;


    }
}