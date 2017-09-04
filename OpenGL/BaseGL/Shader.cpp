#include "..\Global\stdafx.h"
#include "Shader.h"
#include "..\Global\gl.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>

#include "..\Camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm\gtc\type_ptr.hpp>



unsigned int gl::Shader::PROGRAM_COUNT;
unsigned int gl::Shader::MODULE_COUNT;
gl::Index gl::Shader::currentShaderProgram;

std::vector<gl::Shader::Program> gl::Shader::allPrograms;
std::unordered_map<std::string, gl::Index> gl::Shader::shaderProgramLookup;
std::vector<gl::Shader::Module> gl::Shader::allModules;
std::unordered_map<std::string, gl::Index> gl::Shader::moduleLookup;



gl::Index gl::Shader::newModule(string pFileName)
{
	Index index;
	index.index = MODULE_COUNT++;
	moduleLookup.insert(std::pair<string, Index>(pFileName, index));
	allModules.push_back(Module(pFileName));
	return index;
}

gl::Index gl::Shader::newProgram(string pProgramName, gl::Index pVertexShaderID, gl::Index pFragmentShaderID)
{
	Index index = addProgram(pProgramName);
	allPrograms[index.index].type = ProgramType::Basic;
	allPrograms[index.index].stages[0] = pVertexShaderID;
	allPrograms[index.index].stages[1] = pFragmentShaderID;
	allPrograms[index.index].shaderCount = 2;
	return index;
}

gl::Index gl::Shader::newProgram(string pProgramName, gl::Index pVertexShaderID, gl::Index pFragmentShaderID, gl::Index pGeometryShaderID)
{
	Index index = addProgram(pProgramName);
	allPrograms[index.index].type = ProgramType::Geometry;
	allPrograms[index.index].stages[0] = pVertexShaderID;
	allPrograms[index.index].stages[1] = pFragmentShaderID;
	allPrograms[index.index].stages[2] = pGeometryShaderID;
	allPrograms[index.index].shaderCount = 3;
	return index;
}

gl::Index gl::Shader::newProgram(string pProgramName, gl::Index pComputeShaderID)
{
	Index index = addProgram(pProgramName);
	allPrograms[index.index].type = ProgramType::Compute;
	allPrograms[index.index].stages[0] = pComputeShaderID;
	allPrograms[index.index].shaderCount = 1;
	return index;
}

gl::Index gl::Shader::addProgram(string pProgramName)
{
	Program program;
	program.ID = glCreateProgram();
	program.name = pProgramName;
	Index index;
	index.index = PROGRAM_COUNT++;
	index.ID = program.ID;
	shaderProgramLookup.insert(std::pair<string, Index>(pProgramName, index));
	allPrograms.push_back(program);
	
	return index;
}

void gl::Shader::use(unsigned int pID)
{
	currentShaderProgram.ID = pID;
	glUseProgram(pID);
}

void gl::Shader::use(string pProgramName)
{
	use(shaderProgramLookup.find(pProgramName)->second.ID);
}

void gl::Shader::unuse()
{
	currentShaderProgram = {};
	glUseProgram(0);
}


