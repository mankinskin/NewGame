#pragma once
#include "..\..\Global\gl.h"
#include <GL\glew.h>
#include <string>
#include <unordered_map>
#include <glm\glm.hpp>
#include "..\VAO.h"

namespace gl {
    namespace Shader {
	size_t createModule(std::string pFileName);
	size_t createProgram(std::string pProgramName);
	size_t newProgram(std::string pProgramName, size_t pVertexModuleIndex, size_t pFragmentModuleIndex);
	size_t newProgram(std::string pProgramName, size_t pVertexModuleIndex, size_t pFragmentModuleIndex, size_t pGeometryModuleIndex);
	size_t newProgram(std::string pProgramName, size_t pComputeModuleIndex);
	void use(std::string pProgramName);
	void use(size_t pID);
	void unuse();
	void addVertexAttribute(size_t pProgram, std::string pAttributeName, size_t pAttributeIndex);
	void addVertexAttribute(std::string pProgramName, std::string pAttributeName, size_t pAttributeIndex);

	void bindUniformBufferToShader(std::string pProgramName, size_t pTargetStorageIndex, std::string pBlockName);
	void bindUniformBufferToShader(size_t pProgram, size_t pTargetStorageIndex, std::string pBlockName);
	void bindUniformBufferToShader(size_t pProgram, VAO::Storage& pStorage, std::string pBlockName);

	void setUniform(size_t& pProgram, std::string& pUniformName, int& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, size_t& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, float& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::ivec3& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::uvec3& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::vec3& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::mat3& pValue, bool pTranspose = 0);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::ivec4& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::uvec4& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::vec4& pValue);
	void setUniform(size_t& pProgram, std::string& pUniformName, glm::mat4& pValue, bool pTranspose = 0);
	template<typename T>
	void setUniform(size_t& pProgram, std::string pUniformName, T pValue)
	{
	    setUniform(pProgram, pUniformName, pValue);
	}

    }
}