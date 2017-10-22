#include "stdafx.h"
#include "gl.h"
#include <App\Global\Debug.h>
#include <conio.h>
#include <App\Global\App.h>
#include <array>
#include "../BaseGL/Shader/Shader_Loader.h"
#include "../BaseGL/Shader/Shader.h"
#include "..\Camera.h"
#include "glDebug.h"
#include <glm\gtc\type_ptr.hpp>
#include "..\BaseGL\Texture.h"
#include "..\GUI\Text\Font_Loader.h"
#include "..\GUI\UI\GUI.h"
#include "..\GUI\UI\Quad.h"
#include "../Models/Render.h"
#include "../Models/Models.h"
#include "../Models/Model_Loader.h"
#include "../GUI/Text/Text.h"
#include "../BaseGL/Framebuffer.h"
#include "../Lighting/Lights.h"
#include "../GUI/UI/Colored_Quads.h"
#include "../GUI/UI/Buttons.h"
#include "../GUI/UI/Line.h"
#include <App/World/EntityRegistry.h>

int gl::MAX_WORK_GROUP_COUNT = 0;
glm::ivec3 gl::MAX_WORK_GROUP_SIZE = {};
size_t gl::MAX_LIGHT_COUNT = 100;
std::vector<std::string> gl::EXTENSION_LIST = {};
int gl::EXTENSIONS_SUPPORTED_NUM = 0;
int gl::OPENGL_VERSION[2] = {};
std::string gl::GLSL_VERSION = "";
std::string gl::SYSTEM_RENDERER = "";
float gl::resolution = 1.0f;
int gl::MAX_TEXTURE_UNIT_COUNT;
size_t gl::screenWidth = 0;
size_t gl::screenHeight = 0;
size_t gl::quadVBO = 0;
size_t gl::quadEBO = 0;
size_t gl::generalUniformBuffer = 0;
size_t gl::screenQuadVAO;
size_t gl::screenShaderProgram;


void gl::init()
{
	initGLEW();
	getOpenGLInitValues();
	Debug::init();
	Camera::init();
	initFramebuffers();
	initShaders();
	initGeneralBuffers();

	initGUI();
	initModels();
	initLighting();
	bindUniformBufferLocations();


	//-----------
	size_t gridwidth = 15;
	glm::vec3 gridpos = glm::vec3(0.0f, 0.0f, 0.0f);
	float gridscale = 10.0f;
	std::vector<size_t> spheres(gridwidth*gridwidth);
	EntityRegistry::createEntities(gridwidth*gridwidth, &spheres[0]);

	for (size_t x = 0; x < gridwidth; ++x) {
		for (size_t z = 0; z < gridwidth; ++z) {
			EntityRegistry::setPos(spheres[x * gridwidth + z], glm::vec3(gridpos.x + (float)x*gridscale, gridpos.y, gridpos.z + (float)z*gridscale));
			EntityRegistry::setScale(spheres[x * gridwidth + z], glm::vec3(0.2f, 0.2f, 0.2f));
		}
	}

	EntityRegistry::updateMatrices();
	gl::Models::getModel(0).addInstances({ spheres });
	gl::Models::revalidateModelMeshOffsets();
	gl::Models::revalidateModelEntityOffsets();
	//----------

	Debug::getGLError("gl::init()4");
	App::Debug::printErrors();
}

void gl::initFramebuffers()
{
	Texture::initGBuffer();
	Texture::initGUIFBO();
}

void gl::initShaders()
{
	//include shaders
	Debug::initDebugShader();
	Models::initNormalShader();
	Models::initMeshShader();
	Lighting::initLightShader();
	GUI::initButtonIndexShader();
	GUI::initColorQuadShaders();
	GUI::Text::initFontShader();
	GUI::initLineShader();
	Shader::Loader::buildShaderPrograms();
}

void gl::initGeneralBuffers()
{
	initGeneralUniformBuffer();
	initGeneralQuadVBO();
}

void gl::initGUI()
{
	gl::GUI::init();
	gl::GUI::createColor(glm::vec4(0.0, 0.0, 0.0, 1.0), "black");
	gl::GUI::createColor(glm::vec4(1.0, 1.0, 1.0, 1.0), "white");
	gl::GUI::createColor(glm::vec4(1.0, 0.0, 0.0, 1.0), "red");
	gl::GUI::createColor(glm::vec4(0.0, 1.0, 0.0, 1.0), "green");
	gl::GUI::createColor(glm::vec4(0.0, 0.0, 1.0, 1.0), "blue");
	gl::GUI::createColor(glm::vec4(0.0, 0.0, 0.0, 0.0), "none");
	gl::GUI::createColor(glm::vec4(0.5, 0.5, 0.5, 1.0), "grey");
	gl::GUI::createColor(glm::vec4(0.2, 0.2, 0.2, 1.0), "dark_grey");
	gl::GUI::createColor(glm::vec4(0.7, 0.7, 0.7, 1.0), "light_grey");
	GUI::storeGUIColors();
	GUI::initQuadBuffer();
	GUI::initColorQuadVAOs();
	GUI::initButtonBuffer();
	GUI::initLineVAO();
	/*FONTS
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
	Generic
	justabit
	BetterPixels
	UnnamedFont
	TinyUnicode
	Hack-Regular
	DroidSansMonoSlashed
	*/
	GUI::Text::Initializer::initFreeType();
	GUI::Text::initFontVAO();
	GUI::Text::Initializer::includeFont("Generic.ttf", 12, 30, 200, 0, 1);
	GUI::Text::Initializer::loadFonts();
}

void gl::initModels()
{
	Models::initMeshVAO();
	loadModels();
	Models::fillMeshVAO();
}

void gl::initLighting()
{
	Lighting::initLightVAO();
	Lighting::initLightDataBuffer();
}

void gl::bindUniformBufferLocations()
{
	//bind uniform buffers to shaders

	Lighting::setupLightShader();
	Models::setupMeshShader();
	GUI::setupButtonIndexShader();
	GUI::setupLineShader();
	GUI::setupColorQuadShaders();

	//Shader::bindUniformBufferToShader(Debug::lineShaderID, generalUniformBuffer, "GeneralUniformBuffer");
	App::Debug::printErrors();
}

void gl::setViewport(App::ContextWindow::Window& pViewport) {
	screenWidth = pViewport.width;
	screenHeight = pViewport.height;
	glViewport(0, 0, screenWidth, screenHeight);
}

void gl::getOpenGLInitValues()
{
	VAO::STREAM_FLAGS = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
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

	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &VAO::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &VAO::UNIFORM_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &VAO::MAX_UNIFORM_BUFFER_BINDINGS);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT_COUNT);
	glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &VAO::MIN_MAP_BUFFER_ALIGNMENT);

	setViewport(App::mainWindow);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Debug::getGLError("gl::initOpenGL()");
}

void gl::initGLEW() {
	puts("Initializing OpenGL(GLEW)...\n");
	size_t glew = glewInit();
	if (glew != GLEW_OK) {

		App::Debug::pushError(std::string("\nApp::init() - Unable to initialize GLEW (glewInit() return code: %i)\nGLEW Error Log\n %s"
			+ glew) + std::string((const char*)glewGetErrorString(glew)), App::Debug::Error::Severity::Fatal);
		while (!_getch()) {}
		exit(glew);
	}
}

void gl::initGeneralQuadVBO()
{
	/*        Colored-Quad
	2---3     0,0------1,0
	|   |      |        |
	|   |      |        |
	|   |      |        |
	0---1     0,1------1,1
	*/
	float varr[4 * 2] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	size_t iarr[6] = {
		2, 0, 1, 1, 3, 2
	};
	quadVBO = VAO::createStorage(sizeof(float) * 4 * 2, &varr[0], 0);
	quadEBO = VAO::createStorage(sizeof(size_t) * 6, &iarr[0], 0);
}

void gl::initGeneralUniformBuffer()
{
	//General Uniform buffer Contents
	//projectionMatrix
	//viewMatrix
	//camera position
	//
	size_t generalUniformDataSize = sizeof(float) * (16 + 16 + 4);

	generalUniformBuffer = VAO::createStorage(generalUniformDataSize, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(generalUniformBuffer, GL_MAP_WRITE_BIT);
	VAO::bindStorage(GL_UNIFORM_BUFFER, generalUniformBuffer);
	Debug::getGLError("gl::initGeneralUniformBuffer()1:");
}

void gl::updateGeneralUniformBuffer()
{
	std::vector<float> generalUniformData(36);

	std::memcpy(&generalUniformData[0], glm::value_ptr(Camera::getProjection()), sizeof(float) * 16);
	std::memcpy(&generalUniformData[16], glm::value_ptr(Camera::getView()), sizeof(float) * 16);
	std::memcpy(&generalUniformData[32], glm::value_ptr(Camera::getPos()), sizeof(float) * 3);
	VAO::streamStorage(generalUniformBuffer, sizeof(float) * 36, &generalUniformData[0]);

	Debug::getGLError("gl::update():");
}

void gl::loadModels()
{
	Models::Loader::includeModel("sphere.3ds");
	Models::Loader::loadModelFiles();
}

