#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace gl {
	namespace Shader {
		namespace Data {

			struct Storage {
				Storage() {}
				unsigned int ID = 0;
				unsigned int capacity = 0;
				unsigned int bufferFlags = 0;
				unsigned int streamIndex = 0;
				unsigned int target = 0;
				unsigned int binding = 0;

				unsigned int vaoID = 0;
				unsigned int stride = 0;
			};


			unsigned int createStorage(unsigned int pCapacity, const void* pData, unsigned int pFlags);
			
			struct Stream{
				Stream() {}
				unsigned int mapFlags;
				void* mappedPtr;
				unsigned int alignment = 16;
				unsigned int updateOffset = 0;
				unsigned int lastUpdateSize = 0;
			};

			unsigned int createStream(unsigned int pStorageIndex, unsigned int pMapFlags);

			
			unsigned int getStorageID(unsigned int pStorageIndex);
			void streamStorage(unsigned int pStorageIndex, unsigned int pUploadSize, void * pData);
			void streamStorage(Storage& pStorage, unsigned int pUploadSize, void * pData);
			void * mapStorage(unsigned int pStorageIndex, unsigned int pFlags);
			void * mapStorage(Storage& pStorage, unsigned int pFlags);
			void bindStorage(unsigned int pStorageIndex, unsigned int pTarget);
			void bindStorage(unsigned int pTarget, Storage& pStorage);
			void bindVertexArrayVertexStorage(unsigned int pStorageIndex, unsigned int pVAO, unsigned int pTarget, unsigned int pBinding, unsigned int pStride);
			void bindVertexArrayVertexStorage(Storage& pStorage, unsigned int pVAO, unsigned int pTarget, unsigned int pBinding, unsigned int pStride);
			void initVertexAttrib(unsigned int pVAO, unsigned int pBindingIndex, unsigned int pAttributeIndex, unsigned int pCount, unsigned int pType, unsigned int pOffset, unsigned int pNormalize = false);
			
			extern int MIN_MAP_BUFFER_ALIGNMENT;
			extern int STREAM_FLAGS;
			extern int UNIFORM_BUFFER_OFFSET_ALIGNMENT;
			extern int SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
			extern int MAX_UNIFORM_BUFFER_BINDINGS;
			extern std::unordered_map<unsigned int, unsigned int> bufferTargetBinds;
			extern std::vector<Storage> allStorages;
			extern std::vector<Stream> allStreams;

			void setUniform(unsigned int pProgram, std::string pUniformName, int& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, unsigned int& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, float& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::ivec3& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::uvec3& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::vec3& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::mat3& pValue, bool pTranspose = 0);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::ivec4& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::uvec4& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::vec4& pValue);
			void setUniform(unsigned int pProgram, std::string pUniformName, glm::mat4& pValue, bool pTranspose = 0);
			template<typename T>
			void setUniform(unsigned int pProgram, std::string pUniformName, T pValue)
			{
				setUniform(pProgram, pUniformName, pValue);
				gl::Debug::getGLError("setUniform");
			}

			void addVertexAttribute(unsigned int pProgram, std::string pAttributeName, unsigned int pAttributeIndex);
			void addVertexAttribute(std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex);
			
			void bindBufferToUniformBlock(std::string pProgramName, unsigned int pTargetStorageIndex, std::string pBlockName);
			void bindBufferToUniformBlock(unsigned int pProgram, unsigned int pTargetStorageIndex, std::string pBlockName);
			void bindBufferToUniformBlock(unsigned int pProgram, Storage& pStorage, std::string pBlockName);
		}
	}
}