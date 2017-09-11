#include "stdafx.h"
#include "App.h"
#include "Debug.h"
#include "..\Input\Input.h"
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
#include <OpenGL\Render\Model.h>
#include <OpenGL/Render/Render.h>
App::State App::state = App::State::Init;
App::ContextWindow::Window App::mainWindow = App::ContextWindow::Window();
double App::timeFactor = 1.0;
double App::lastFrameMS = 0;
double App::lastFrameLimitedMS = 0;
double App::totalMS = 0;
double App::targetFrameMS = 16.0;


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
	EntityRegistry::initEntities();
	
	gl::Model::addModelInstances(0, { 0 });

	gl::Model::revalidateEntityOffsets();
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


void App::mainMenu()
{
	using gl::GUI::Text::Textbox;
	using gl::GUI::Text::String;
	using gl::GUI::Text::createTextbox;
	using gl::GUI::Text::appendTextboxString;
	using gl::GUI::Text::createTextStyle;
	glClearColor(0.05f, 0.0f, 0.0f, 1.0f);

	gl::GUI::reserveQuadSpace(2);
	unsigned int startButtonQuad = gl::GUI::createQuad(glm::vec2(-1.0f, -0.8f), glm::vec2(0.2f, 0.05f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	unsigned int quitButtonQuad = gl::GUI::createQuad(glm::vec2(-1.0f, -0.9f), glm::vec2(0.2f, 0.05f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	Input::addButton(startButtonQuad);
	Input::addButton(quitButtonQuad);
	App::Input::calculateDetectionRanges();
	gl::GUI::updateGUI();

	gl::GUI::Text::allTextboxes.reserve(2);
	gl::GUI::Text::allTextboxPositions.reserve(2);
	gl::GUI::Text::allTextboxSizes.reserve(2);
	gl::GUI::Text::allTextboxMetrics.reserve(2);
	
	
	//gl::GUI::Text::initStyleBuffer();
	String quitProgramStr("QUIT");
	String runProgramStr("PLAY");
	
	unsigned int tb_met = gl::GUI::Text::createTextboxMetrics(1.0f, 1.0f, 1.0f, 1.0f);
	
	unsigned int tb1 = createTextbox(glm::vec2(-1.0f, -0.8f), glm::vec2(0.2f, 0.05f), tb_met, TEXT_LAYOUT_BOUND_LEFT, 0.003f);
	unsigned int tb2 = createTextbox(glm::vec2(-1.0f, -0.9f), glm::vec2(0.2f, 0.05f), tb_met, TEXT_LAYOUT_BOUND_LEFT, 0.003f);
	appendTextboxString(tb1, runProgramStr);
	appendTextboxString(tb2, quitProgramStr);
	
	gl::GUI::Text::loadChars();
	
	
	while (state == App::MainMenu) {
		gl::GUI::Text::updateCharStorage();//why does this only work if i update it each frame?!
		
		App::Input::fetchGLFWEvents();
		Input::fetchButtonEvents();
		App::Input::checkEvents();
		App::Input::callFunctions();
		Input::end();
		gl::GUI::updateGUI();

		gl::frame();

		
		Debug::printErrors();
		updateTime();
		updateTimeFactor();
		limitFPS();

		Debug::printInfo();
	}
	gl::GUI::clearBuffers();
	App::Input::clearDetectionRanges();
	gl::GUI::Text::clearCharStorage();
}

void App::run() {
	state = App::State::Running;
}

void App::quit() {
	state = App::State::Exit;
}

void App::frameLoop()
{
	glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
	
	
	gl::GUI::Text::loadChars();
	gl::GUI::Text::updateCharStorage();

	gl::GUI::updateGUI();
	Debug::printErrors();
	while (state == App::State::Running) {

		
		Input::fetchGLFWEvents();
		
		App::Input::checkEvents();
		App::Input::callFunctions();
		Input::end();
		gl::Camera::look(Input::cursorFrameDelta);
		gl::Camera::update();
		gl::GUI::updateGUI();
		Debug::printErrors();
		EntityRegistry::setPos(0, glm::vec3(gl::Camera::pos.x, 0.0f, gl::Camera::pos.z));
 		gl::updateGeneralUniformBuffer();
		gl::Render::updateBuffers();
		Debug::printErrors();

		gl::frame();

		updateTime();
		updateTimeFactor();
		limitFPS();
		
		Debug::printInfo();
	}
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