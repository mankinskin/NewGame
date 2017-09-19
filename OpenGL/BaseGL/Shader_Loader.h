#pragma once
#include <string>

namespace gl {
	struct Index;
	namespace Shader {
		namespace Loader {
			extern std::string SHADER_DIR;

			void buildShaderPrograms();
			void setShaderDirectory(std::string pDirectory);
			void resetShaderDirectory();

			void compileModule(Index pModuleID);
			void linkProgram(Index pProgramID);
			void compileAndLink();
		}
	}
}