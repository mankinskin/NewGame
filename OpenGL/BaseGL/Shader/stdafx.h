#pragma once
#include <string>
#include <vector>
#include <unordered_map>
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
	    size_t stages[4];
	    size_t shaderCount;
	};

	struct Module {
	    Module(std::string& pFileName) :fileName(pFileName), content(""), ID(0), type(ModuleType::Vertex) {}
	    std::string content;
	    std::string fileName;
	    GLuint ID;
	    ModuleType type;
	};
	extern size_t currentShaderProgram;
	extern std::vector<Program> allPrograms;
	extern std::unordered_map<std::string, size_t> shaderProgramLookup;
	extern std::vector<Module> allModules;
	extern std::unordered_map<std::string, size_t> moduleLookup;
	namespace Loader {
	    extern std::string SHADER_DIR;
	}
    }
}