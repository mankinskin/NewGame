#pragma once
#include <string>

namespace gl {
    struct Index;
    namespace Shader {
	namespace Loader {


	    void buildShaderPrograms();
	    void setShaderDirectory(std::string& pDirectory);
	    void resetShaderDirectory();

	    void compileModule(size_t pModuleIndex);
	    void linkProgram(size_t pProgramID);
	    void compileAndLink();
	}
    }
}