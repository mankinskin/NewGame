#include "stdafx.h"
#include "gl.h"
#include <App\Global\Debug.h>
#include <conio.h>
#include <App\Global\App.h>
#include <App\Input.h>
#include <array>
#include "..\BaseGL\Shader_Loader.h"
#include "..\Camera.h"
#include "glDebug.h"
#include <glm\gtc\type_ptr.hpp>
#include "..\BaseGL\Shader_Data.h"
#include "..\BaseGL\Texture.h"
#include "..\UI\Font_Loader.h"
#include "..\UI\GUI.h"


int gl::MAX_WORK_GROUP_COUNT = 0;
glm::ivec3 gl::MAX_WORK_GROUP_SIZE = {};
unsigned int gl::MAX_LIGHT_COUNT = 100;
std::vector<std::string> gl::EXTENSION_LIST = {};
int gl::EXTENSIONS_SUPPORTED_NUM = 0;
int gl::OPENGL_VERSION[2] = {};
std::string gl::GLSL_VERSION = "";
std::string gl::SYSTEM_RENDERER = "";
float gl::resolution = 1.0f;
int gl::MAX_TEXTURE_UNIT_COUNT;
unsigned int gl::screenPixelWidth = 0;
unsigned int gl::screenPixelHeight = 0;
unsigned int gl::mergeComputeShader = 0;
unsigned int gl::finalMergeShader;
unsigned int gl::mainFrame = 0;
unsigned int gl::mainFBO  = 0;
unsigned int gl::quadVBO = 0;
unsigned int gl::quadEBO = 0; 
unsigned int gl::generalUniformBuffer = 0;


void gl::init()
{
	initGLEW();
	getOpenGLInitValues();
	Debug::generateDebugGrid("grid1.0", 1.0f, 1000, 1.0f, 1.0f, 1.0f, 0.9f);
	Debug::generateDebugGrid("grid0.5", 0.5f, 2000, 1.0f, 1.0f, 1.0f, 0.3f);
	Debug::initCoordinateSystem("coord");
	Debug::init();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	Camera::init();
	initQuadBuffers();
	gl::Debug::getGLError("gl::init():");
	//init framebuffers
	
	
	//include shaders
	GUI::initGUIShaders();
	GUI::initGUIBuffers();
	GUI::Text::Initializer::initFreeType();
	gl::Debug::getGLError("gl::init()1:");
	App::Debug::printErrors();
	GUI::Text::initFontVAO();
	GUI::Text::initFontShader();
	Debug::initDebugShader();
	Shader::Loader::buildShaderPrograms();
	Debug::getGLError("gl::init()3");
	App::Debug::printErrors();
	/*FONTS
	C64_Pro_Mono,
	times,
	FreeSans,
	Input_Regular_Mono,
	Input_Light_Mono,
	FreeMono,
	C64_Pro_Mono
	C64_Pro_Regular,
	Ubuntu_Regular_Mono,
	SourceCodePro_Regular,
	SourceCodePro_Medium,
	VCR_OSD_MONO
	*/
	GUI::Text::Initializer::includeFont("Ubuntu_Regular_Mono.ttf", 15, 30, 200, FONT_LOAD_DT, 4);
	GUI::Text::Initializer::loadFonts();
	
	
	//bind uniform buffers to shaders
	initGeneralUniformBuffer();
	
	Shader::Data::bindBufferToUniformBlock(GUI::Text::glyphShapeProgram, generalUniformBuffer, "GeneralUniformBuffer");
	
	Shader::Data::bindBufferToUniformBlock(Debug::lineShaderID, generalUniformBuffer, "GeneralUniformBuffer");
	
	Shader::Data::bindBufferToUniformBlock(gl::GUI::guiQuadShader, gl::GUI::guiPositionBuffer, "PositionBuffer");
	Shader::Data::bindBufferToUniformBlock(gl::GUI::guiQuadShader, gl::GUI::guiSizeBuffer, "SizeBuffer");
	Shader::Data::bindBufferToUniformBlock(gl::GUI::guiQuadShader, gl::GUI::guiUVBuffer, "UVBuffer");
	Shader::Data::bindBufferToUniformBlock(gl::GUI::guiQuadShader, gl::GUI::guiColorBuffer, "ColorBuffer");
	
	
	
	
	Debug::getGLError("gl::init()4");
	App::Debug::printErrors();
}

void gl::configure()
{
	setViewport(App::mainWindow);
	
}

void gl::setViewport(App::ContextWindow::Window& pViewport) {
	screenPixelWidth = pViewport.width;
	screenPixelHeight = pViewport.height;
	glViewport(0, 0, screenPixelWidth, screenPixelHeight);
}

void gl::getOpenGLInitValues()
{
	Shader::Data::STREAM_FLAGS = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glGetIntegerv(GL_MAJOR_VERSION, &OPENGL_VERSION[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OPENGL_VERSION[1]);
	GLSL_VERSION = std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	SYSTEM_RENDERER = std::string((char*)glGetString(GL_RENDERER));

	glGetIntegerv(GL_NUM_EXTENSIONS, &EXTENSIONS_SUPPORTED_NUM);
	EXTENSION_LIST.resize(EXTENSIONS_SUPPORTED_NUM);
	for (int k = 0; k < EXTENSIONS_SUPPORTED_NUM; ++k) {
		EXTENSION_LIST[k] = std::string((char*)glGetStringi(GL_EXTENSIONS, k));
	}

	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 0, &MAX_WORK_GROUP_SIZE.x);
	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 1, &MAX_WORK_GROUP_SIZE.y);
	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 2, &MAX_WORK_GROUP_SIZE.z);

	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &Shader::Data::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &Shader::Data::UNIFORM_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &Shader::Data::MAX_UNIFORM_BUFFER_BINDINGS);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT_COUNT);
	glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &Shader::Data::MIN_MAP_BUFFER_ALIGNMENT);
	screenPixelWidth = App::mainWindow.width;
	screenPixelHeight = App::mainWindow.height;

	//glBindImageTexture(0, mainFrame, 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	Debug::getGLError("gl::initOpenGL()");
}

void gl::initGLEW() {
	unsigned int glew = glewInit();
	if (glew != GLEW_OK) {

		App::Debug::pushError(std::string("\nApp::init() - Unable to initialize GLEW (glewInit() return code: %i)\nGLEW Error Log\n %s"
			+ glew) + std::string((const char*)glewGetErrorString(glew)), App::Debug::Error::Severity::Fatal);
		while (!_getch()) {}
		exit(glew);
	}
}

void gl::initQuadBuffers()
{

	/*
	0---2
	|  /|
	| / |
	|/  |
	1---3
	*/
	float varr[4 * 2] = {
		0.0f, 0.0f, 
		0.0f, 1.0f,
		1.0f, 0.0f, 
		1.0f, 1.0f
	};
	unsigned int iarr[6] = {
		0, 1, 2, 2, 1, 3
	};
	quadVBO = Shader::Data::createStorage(sizeof(float) * 4 * 2, &varr[0], 0) + 1;
	quadEBO = Shader::Data::createStorage(sizeof(unsigned int) * 6, &iarr[0], 0) + 1;
}

void gl::initGeneralUniformBuffer()
{
	unsigned int generalUniformDataSize = sizeof(float) * 16 * 2;
	
	generalUniformBuffer = Shader::Data::createStorage(generalUniformDataSize * 16, nullptr, GL_MAP_WRITE_BIT | Shader::Data::STREAM_FLAGS);
	Shader::Data::createStream(generalUniformBuffer,  GL_MAP_WRITE_BIT);
	Shader::Data::bindStorage(GL_UNIFORM_BUFFER, generalUniformBuffer);
	Debug::getGLError("gl::initGeneralUniformBuffer()1:");
}

void gl::updateGeneralUniformBuffer()
{
	std::vector<float> generalUniformData(32);
	
	std::memcpy(&generalUniformData[0], glm::value_ptr(Camera::infiniteProjectionMatrix), sizeof(float) * 16);
	std::memcpy(&generalUniformData[16], glm::value_ptr(Camera::viewMatrix), sizeof(float) * 16);
	
	Shader::Data::streamStorage(generalUniformBuffer, sizeof(float) * 32, &generalUniformData[0]);
	
	Debug::getGLError("gl::update():");
}

void gl::frame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Debug::drawGrid();
	GUI::renderGUI();
	GUI::Text::renderGlyphs();
	
	glfwSwapBuffers(App::mainWindow.window);
	Debug::getGLError("Frame");
}


