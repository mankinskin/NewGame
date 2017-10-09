#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "Shader_Loader.h"
#include "Shader.h"
#include <App\Global\Debug.h>
#include <fstream>
#include <sstream>
#include "..\..\Global\glDebug.h"
#define DEFAULT_SHADER_DIRECTORY "..//deps//shaders//"

std::string gl::Shader::Loader::SHADER_DIR = DEFAULT_SHADER_DIRECTORY;



void gl::Shader::Loader::buildShaderPrograms()
{

	compileAndLink();
}

void gl::Shader::Loader::setShaderDirectory(std::string& pDirectory)
{
	SHADER_DIR = pDirectory;
}

void gl::Shader::Loader::resetShaderDirectory()
{
	SHADER_DIR = DEFAULT_SHADER_DIRECTORY;
}

void gl::Shader::Loader::compileModule(unsigned int pModuleIndex)
{
	Module& module = allModules[pModuleIndex];
	if (module.fileName.find(".vert") != std::string::npos) {
		module.ID = glCreateShader(GL_VERTEX_SHADER);
		module.type = ModuleType::Vertex;
	}
	else if (module.fileName.find(".geo") != std::string::npos) {
		module.ID = glCreateShader(GL_GEOMETRY_SHADER);
		module.type = ModuleType::Geometry;
	}
	else if (module.fileName.find(".frag") != std::string::npos) {
		module.ID = glCreateShader(GL_FRAGMENT_SHADER);
		module.type = ModuleType::Fragment;
	}
	else if (module.fileName.find(".comp") != std::string::npos) {
		module.ID = glCreateShader(GL_COMPUTE_SHADER);
		module.type = ModuleType::Compute;
	}
	else {
		App::Debug::pushError("\nShader::loadShader(): invalid shader file name " + module.fileName + "!\nHas to include '.vert', '.frag', '.geo' or '.comp'!", App::Debug::Error::Fatal);
		return;
	}
	gl::Debug::getGLError("GLERROR: Shader::Loader::compileModule()1: at " + module.fileName);
	std::ifstream moduleFile;
	moduleFile.open(SHADER_DIR + module.fileName + ".txt");
	if (moduleFile.fail()) {
		App::Debug::pushError("Failed to compile Shader: Could not open " + SHADER_DIR + module.fileName + ".txt" + "!\n");
		return;
	}

	module.content = static_cast<std::stringstream const&>(std::stringstream() << moduleFile.rdbuf()).str();
	const char* content = module.content.c_str();
	int length = module.content.length();
	glShaderSource(module.ID, 1, &content, &length);
	glCompileShader(module.ID);
	int success = 0;
	glGetShaderiv(module.ID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int maxLength = 0;
		glGetShaderiv(module.ID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(module.ID, maxLength, &maxLength, &errorLog[0]);
		App::Debug::pushError("Failed to compile " + module.fileName + "\nOpenGL Error Log: " + string(&(errorLog[0])) + "\n", App::Debug::Error::Fatal);
		return;
	}
	allModules[pModuleIndex] = module;
	gl::Debug::getGLError("GLERROR: Shader::Loader::compileModule(): at " + module.fileName);
}

void gl::Shader::Loader::linkProgram(unsigned int pProgramIndex)
{
	Program& program = allPrograms[pProgramIndex];

	if (program.type == ProgramType::Compute) {
		program.stages[0] = allModules[program.stages[0]].ID;
		glAttachShader(program.ID, program.stages[0]);
	}
	else
	{
		for (unsigned int i = 0; i < program.shaderCount; ++i) {
			program.stages[i] = allModules[program.stages[i]].ID;
			glAttachShader((GLuint)program.ID, (GLuint)program.stages[i]);
		}
	}
	glLinkProgram(program.ID);
	GLint success = 0;
	glGetProgramiv(program.ID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program.ID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(program.ID, maxLength, &maxLength, &errorLog[0]);

		glDeleteProgram(program.ID);


		App::Debug::pushError("!!!\nError when linking program: " + program.name + " \nopenGL Error Log: " + &(errorLog[0]), App::Debug::Error::Fatal);

	}

	for (unsigned int i = 0; i < program.shaderCount; ++i) {
		glDetachShader(program.ID, program.stages[i]);
	}
	gl::Debug::getGLError("GLERROR: ShaderProgram::linkProgram(): at " + program.name);
}

void gl::Shader::Loader::compileAndLink()
{
	gl::Debug::getGLError("GLERROR: before compileAndLink():");
	for (unsigned int s = 0; s < allModules.size(); ++s) {
		compileModule(s);
	}
	for (unsigned int p = 0; p < allPrograms.size(); ++p) {
		linkProgram(p);
	}
}
