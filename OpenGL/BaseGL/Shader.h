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
			unsigned int stages[4];
			unsigned int shaderCount;
		};
		
		struct Module {
			Module(std::string& pFileName) :fileName(pFileName), content(""), ID(0), type(ModuleType::Vertex){}
			std::string content;
			std::string fileName;
			GLuint ID;
			ModuleType type;
		};

		extern unsigned int currentShaderProgram;
		extern std::vector<Module> allModules;
		extern std::unordered_map<std::string, unsigned int> moduleLookup;//use this to search for modules by name string
		extern std::vector<Program> allPrograms;
		extern std::unordered_map<std::string, unsigned int> shaderProgramLookup;
		
		unsigned int createModule(std::string pFileName);
		unsigned int createProgram(std::string pProgramName);
		unsigned int newProgram(std::string pProgramName, unsigned int pVertexModuleIndex, unsigned int pFragmentModuleIndex);
		unsigned int newProgram(std::string pProgramName, unsigned int pVertexModuleIndex, unsigned int pFragmentModuleIndex, unsigned int pGeometryModuleIndex);
		unsigned int newProgram(std::string pProgramName, unsigned int pComputeModuleIndex);
		void use(std::string pProgramName);
		void use(unsigned int pID);
		void unuse();
		void addVertexAttribute(unsigned int pProgram, std::string pAttributeName, unsigned int pAttributeIndex);
		void addVertexAttribute(std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex);

		void bindUniformBufferToShader(std::string pProgramName, unsigned int pTargetStorageIndex, std::string pBlockName);
		void bindUniformBufferToShader(unsigned int pProgram, unsigned int pTargetStorageIndex, std::string pBlockName);
		void bindUniformBufferToShader(unsigned int pProgram, VAO::Storage& pStorage, std::string pBlockName);
		
		void setUniform(unsigned int& pProgram, std::string& pUniformName, int& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, unsigned int& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, float& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::ivec3& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::uvec3& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::vec3& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::mat3& pValue, bool pTranspose = 0);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::ivec4& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::uvec4& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::vec4& pValue);
		void setUniform(unsigned int& pProgram, std::string& pUniformName, glm::mat4& pValue, bool pTranspose = 0);
		template<typename T>
		void setUniform(unsigned int& pProgram, std::string pUniformName, T pValue)
		{
			setUniform(pProgram, pUniformName, pValue);
		}
	
	}
}