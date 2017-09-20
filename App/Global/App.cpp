#include "stdafx.h"
#include "App.h"
#include "Debug.h"
#include "..\Input\Input.h"
#include "..\Input\Mouse.h"
#include "..\Input\Buttons.h"
#include <conio.h>
#include <thread>
#include <chrono>
#include <OpenGL\Global\gl.h>
#include <OpenGL\UI\Text.h>
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <OpenGL\UI\Font_Loader.h>
#include <OpenGL\UI\GUI.h>
#include "../World/EntityRegistry.h"
#include <OpenGL\Render\Models.h>
#include <OpenGL\Render\Render.h>
#include <OpenGL\Lighting\Lights.h>
App::State App::state = App::State::Init;
App::ContextWindow::Window App::mainWindow = App::ContextWindow::Window();
double App::timeFactor = 1.0;
double App::lastFrameMS = 0;
double App::lastFrameLimitedMS = 0;
double App::totalMS = 0;
double App::targetFrameMS = 16.0;

glm::vec3 App::light_pos = glm::vec3(4.0f, 3.0f, 4.0f);
glm::vec3 App::light_mov = glm::vec3();
void App::init()
{
	state = MainMenu;
	initGLFW();
	//Windows and gl Context
	ContextWindow::initMonitors();
	ContextWindow::primaryMonitor.init();
	mainWindow.setSize(1600, 850);
	mainWindow.init();
	//Input listeners
	Input::init();
	gl::init();
	gl::configure();
	gl::GUI::Text::initStyleBuffer();
	
	Debug::printErrors();
}

void App::initGLFW()
{
	unsigned int glfw = glfwInit();
	if (glfw != GLFW_TRUE) {
		Debug::pushError(("\nApp::init() - Unable to initialize GLFW (glfwInit() return code: %i)\n" + glfw), Debug::Error::Severity::Fatal);
		while (!_getch()) {}
		exit(glfw);
	}
}

void App::initMainMenu() {
	using gl::GUI::Text::Textbox;
	using gl::GUI::Text::String;
	using gl::GUI::Text::createTextbox;
	using gl::GUI::Text::setTextboxString;
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	gl::GUI::reserveQuadSpace(4);
	unsigned int startButtonQuad = gl::GUI::createQuad(glm::vec2(-1.0f, -0.7f), glm::vec2(0.2f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	unsigned int quitButtonQuad = gl::GUI::createQuad(glm::vec2(-1.0f, -0.9f), glm::vec2(0.2f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	unsigned int windowQuad = gl::GUI::createQuad(glm::vec2(-0.9f, 0.9f), glm::vec2(1.2f, 1.2f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	unsigned int tabQuad = gl::GUI::createQuad(glm::vec2(-0.87f, 0.89f), glm::vec2(1.14f, 0.18f), glm::vec4(0.9f, 0.4f, 0.4f, 1.0f));
	Input::addButton(startButtonQuad);
	Input::addButton(quitButtonQuad);
	Input::addButton(tabQuad);
	App::Input::loadButtons();
	Input::initMenuSignals();

	gl::GUI::Text::reserveTextboxSpace(2); 
	gl::GUI::Text::allTextboxMetrics.reserve(2);


	
	String quitProgramStr("QUIT");
	String runProgramStr("PLAY");
	String textStr("Dis is Text\nThis too\nLook here, a longer line!\nNow a line that goes on so long until there is no more space and line breaks have to be inserted automatically.");
	unsigned int tb_met = gl::GUI::Text::createTextboxMetrics(0, 1.0f, 1.0f, 1.0f, 1.0f);

	unsigned int tb1 = createTextbox(startButtonQuad, tb_met, TEXT_LAYOUT_BOUND_LEFT | TEXT_LAYOUT_CENTER_Y, 0.003f);
	unsigned int tb2 = createTextbox(quitButtonQuad, tb_met, TEXT_LAYOUT_BOUND_LEFT | TEXT_LAYOUT_CENTER_Y, 0.003f);
	unsigned int tb3 = createTextbox(glm::vec2(-0.9f, 0.8f), glm::vec2(1.2f, 1.0f), tb_met, TEXT_LAYOUT_BOUND_LEFT , 0.003f);
	setTextboxString(tb1, runProgramStr);
	setTextboxString(tb2, quitProgramStr);
	setTextboxString(tb3, textStr);
	gl::GUI::Text::loadTextboxes();

}

void App::initGameGUI() {
	using gl::GUI::Text::Textbox;
	using gl::GUI::Text::String;
	using gl::GUI::Text::createTextbox;
	using gl::GUI::Text::setTextboxString;

	//gl::GUI::reserveQuadSpace(2);
	//unsigned int menuButtonQuad = gl::GUI::createQuad(glm::vec2(-1.0f, -0.7f), glm::vec2(0.2f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//unsigned int quitButtonQuad = gl::GUI::createQuad(glm::vec2(-1.0f, -0.85f), glm::vec2(0.2f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//Input::addButton(quitButtonQuad);
	//Input::addButton(menuButtonQuad);
	App::Input::loadButtons();
	App::Input::initGameGUISignals();
	gl::GUI::Text::reserveTextboxSpace(2);

	String quitProgramStr("QUIT");
	String mainmenuProgramStr("MAIN MENU");
	
	//unsigned int tb1 = createTextbox(quitButtonQuad, 0, TEXT_LAYOUT_BOUND_LEFT | TEXT_LAYOUT_CENTER_Y, 0.003f);
	//unsigned int tb2 = createTextbox(menuButtonQuad, 0, TEXT_LAYOUT_BOUND_LEFT | TEXT_LAYOUT_CENTER_Y, 0.003f);
	//setTextboxString(tb1, quitProgramStr);
	//setTextboxString(tb2, mainmenuProgramStr);
	gl::GUI::Text::loadTextboxes();

	EntityRegistry::initEntities();

	gl::Models::allModels[0].addInstances({ 0 });

	gl::Models::Model::revalidateEntityOffsets();
}


void App::mainMenuLoop()
{
	initMainMenu();
	while (state == App::MainMenu) {
		gl::GUI::updateGUI();
		gl::GUI::Text::updateCharStorage();//why does this only work if i update it each frame?!
		App::Input::fetchGLFWEvents();
		Input::fetchButtonEvents();
		App::Input::checkEvents();
		App::Input::callFunctions();
		Input::end();

		gl::frameStart();

		gl::GUI::renderGUI();
		gl::GUI::Text::renderGlyphs();

		gl::frameEnd();

		Debug::printErrors();
		updateTime();
		updateTimeFactor();
		limitFPS();

		Debug::printInfo();
	}
	gl::GUI::Text::clearCharStorage();
	gl::GUI::clearBuffers();
	Input::clearButtons();
	
}

void App::run() {
	state = App::State::Running;
}

void App::quit() {
	state = App::State::Exit;
}

void App::mainmenu()
{
	state = App::State::MainMenu;
}

void App::frameLoop()
{
	glClearColor(0.1f, 0.1f, 0.3f, 0.0f);
	initGameGUI();
	
	gl::Lighting::createLight(glm::vec4(light_pos, 1.0f), glm::vec4(1.0f, 0.0f, 0.0, 10.0f));
	gl::Lighting::updateLightIndexRangeBuffer();


	Debug::printErrors();
	while (state == App::State::Running) {
		gl::GUI::updateGUI();
		gl::GUI::Text::updateCharStorage();
		Input::fetchGLFWEvents();
		Input::fetchButtonEvents();
		App::Input::checkEvents();
		App::Input::callFunctions();
		Input::end();
		
		gl::Camera::look(Input::cursorFrameDelta);
		gl::Camera::update();
		Debug::printErrors();
		light_pos += light_mov * 0.2f;
		gl::Lighting::setLightPos(0, light_pos);
		//EntityRegistry::setPos(0, glm::vec3(gl::Camera::pos.x, 0.0f, gl::Camera::pos.z));
 		gl::updateGeneralUniformBuffer();
		gl::Lighting::updateLightDataBuffer();
		
		gl::Render::updateBuffers();
		Debug::printErrors();

		gl::frameStart();

  		
		gl::Debug::drawGrid();
		gl::GUI::renderGUI();
		gl::GUI::Text::renderGlyphs();
		gl::Render::render();
		gl::Lighting::renderLights();
		
		
		
		gl::frameEnd();
		updateTime();
		updateTimeFactor();
		limitFPS();
		
		Debug::printInfo();
	}
	gl::GUI::Text::clearCharStorage();
	gl::GUI::clearBuffers();
	Input::clearButtons();
}



//--Global Time--
void App::updateTime()
{
	double thisFrameMS = (glfwGetTime() * 1000.0);
	lastFrameMS = thisFrameMS - totalMS;
	totalMS = thisFrameMS;
}

void App::limitFPS()
{
	lastFrameLimitedMS = lastFrameMS;
	if (lastFrameMS < targetFrameMS) {
		lastFrameLimitedMS = targetFrameMS;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(targetFrameMS - lastFrameMS)));
	}
}

void App::updateTimeFactor() {
	timeFactor = lastFrameMS / targetFrameMS;
}

void App::setTargetFPS(unsigned int pTargetFPS)
{
	targetFrameMS = (unsigned int)(1000.0f / (float)pTargetFPS);
}
