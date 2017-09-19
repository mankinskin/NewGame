#pragma once
#include "..\Global\gl.h"
#include <GL\glew.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <glm\glm.hpp>
#include "VAO.h"

namespace gl {
	namespace Shader {
			
		enum class ModuleType {
			Vertex,
			Fragment,
			Geometry,
			Compute
		};
		
		enum class ProgramType {
			Basic,
			Geometry,
			Compute
		};
		
		struct Program {
			GLuint ID;
			std::string name;
			ProgramType type;
			Index stages[4];
			unsigned int shaderCount;
		};
		
		struct Module {
			Module(std::string pFileName) {
				fileName = pFileName;
			}
			std::string content;
			std::string fileName;
			GLuint ID;
			ModuleType type;
		};
		
		
		extern unsigned int PROGRAM_COUNT;
		extern unsigned int MODULE_COUNT;
		extern Index currentShaderProgram;
		extern std::vector<Module> allModules;
		extern std::unordered_map<std::string, Index> moduleLookup;//use this to search for modules by name string
		extern std::vector<Program> allPrograms;
		extern std::unordered_map<std::string, Index> shaderProgramLookup;
		
		Index newModule(std::string pFileName);
		Index newProgram(std::string pProgramName, Index pVertexModuleID, Index pFragmentModuleID);
		Index newProgram(std::string pProgramName, Index pVertexModuleID, Index pFragmentModuleID, Index pGeometryModuleID);
		Index newProgram(std::string pProgramName, Index pComputeModuleID);
		Index addProgram(std::string pProgramName);
		void use(std::string pProgramName);
		void use(unsigned int pID);
		void unuse();
		void addVertexAttribute(unsigned int pProgram, std::string pAttributeName, unsigned int pAttributeIndex);
		void addVertexAttribute(std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex);

		void bindUniformBufferToShader(std::string pProgramName, unsigned int pTargetStorageIndex, std::string pBlockName);
		void bindUniformBufferToShader(unsigned int pProgram, unsigned int pTargetStorageIndex, std::string pBlockName);
		void bindUniformBufferToShader(unsigned int pProgram, VAO::Storage& pStorage, std::string pBlockName);
		
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
		}
	
	}
}