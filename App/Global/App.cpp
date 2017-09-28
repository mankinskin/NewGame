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
#include <OpenGL\GUI\Text.h>
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <OpenGL\GUI\Font_Loader.h>
#include <OpenGL\GUI\GUI.h>
#include "../World/EntityRegistry.h"
#include <OpenGL\Models\Models.h>
#include <OpenGL\Models\Render.h>
#include <OpenGL/Lighting\Lights.h>
#include <OpenGL/GUI/Buttons.h>
#include <OpenGL/GUI/Colored_Quads.h>
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
	//gl::GUI::Text::initStyleBuffer();
	
	Debug::printErrors();
}

void App::initGLFW()
{
        std::puts("Initializing GLFW...\n");
	unsigned int glfw = glfwInit();
	if (glfw != GLFW_TRUE) {
		Debug::pushError(("\nApp::init() - Unable to initialize GLFW (glfwInit() return code: %i)\n" + glfw), Debug::Error::Severity::Fatal);
		while (!_getch()) {}
		exit(glfw);
	}
}


void App::mainMenuLoop()
{

        unsigned int quitButtonQuad = gl::GUI::createQuad(-1.0f, -0.8f, 0.2f, 0.1f);
        gl::GUI::colorQuad(quitButtonQuad, 0);
        gl::GUI::Text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
        unsigned int tb = gl::GUI::Text::createTextbox(quitButtonQuad, 0, 0);
        gl::GUI::Text::setTextboxString(tb, gl::GUI::Text::String("QUIT"));
        gl::GUI::Text::loadTextboxes();
        //-----------
        unsigned int sphere = 0;
        EntityRegistry::createEntities(1, &sphere);

        EntityRegistry::setPos(sphere, glm::vec3(1.0f, 0.0f, -4.0f));
        EntityRegistry::updateMatrices();
        gl::Models::allModels[0].addInstances({sphere});
        gl::Models::Model::revalidateMeshOffsets();
        gl::Models::Model::revalidateEntityOffsets();
        //----------
        Input::toggleTrackMouse();
	while (state == App::MainMenu) {

                fetchInput();
                gl::Camera::look(Input::cursorFrameDelta);
                gl::Camera::update();
                gl::updateGeneralUniformBuffer();
                gl::GUI::updateQuadBuffer();
                gl::GUI::updateColoredQuads();
                gl::GUI::Text::updateCharStorage();
                gl::Models::updateBuffers();
                gl::frameStart();
                
                gl::Debug::drawGrid();
                gl::GUI::renderColoredQuads();
                gl::Models::render();
                gl::GUI::Text::renderGlyphs();
                gl::Lighting::renderLights();
                gl::renderScreenQuad();
		gl::frameEnd();

		Debug::printErrors();
		updateTime();
		updateTimeFactor();
		limitFPS();

		Debug::printInfo();
	}
	
}

void App::fetchInput()
{
        Input::fetchEvents();
        Input::checkEvents();
        Input::callFunctions();
        Input::end();
}


void App::frameLoop()
{
	while (state == App::State::Running) {
                fetchInput();
		
                //camera
		gl::Camera::look(Input::cursorFrameDelta);
		gl::Camera::update();

 		gl::updateGeneralUniformBuffer();

		gl::frameStart();

		gl::Debug::drawGrid();
		
		gl::frameEnd();
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
