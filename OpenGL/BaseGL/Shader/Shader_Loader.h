#pragma once
#include <string>

namespace gl {
	struct Index;
	namespace Shader {
		namespace Loader {
			

			void buildShaderPrograms();
			void setShaderDirectory(std::string& pDirectory);
			void resetShaderDirectory();

			void compileModule(unsigned int pModuleIndex);
			void linkProgram(unsigned int pProgramID);
			void compileAndLink();
		}
	}
}