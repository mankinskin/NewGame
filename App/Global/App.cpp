#include "stdafx.h"
#include "App.h"
#include "Debug.h"
#include "..\Input\Input.h"
#include "..\Input\Signal.h"
#include "..\Input\Mouse.h"
#include <conio.h>
#include <thread>
#include <chrono>
#include <OpenGL\Global\gl.h>
#include <OpenGL\GUI\Text\Text.h>
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <OpenGL\GUI\Text\Font_Loader.h>
#include <OpenGL\GUI\UI\GUI.h>
#include "../World/EntityRegistry.h"
#include <OpenGL\Models\Models.h>
#include <OpenGL\Models\Render.h>
#include <OpenGL/Lighting\Lights.h>
#include <OpenGL/GUI/UI/Buttons.h>
#include <OpenGL/GUI/UI/Colored_Quads.h>
#include <OpenGL/BaseGL/Framebuffer.h>
#include <OpenGL\GUI\UI\Line.h>
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
	using namespace Input;
        //Buttons
        unsigned int quitButtonQuad = gl::GUI::createQuad(-1.0f, -0.9f, 0.2f, 0.05f);
        unsigned int playButtonQuad = gl::GUI::createQuad(-1.0f, -0.7f, 0.2f, 0.1f);
       
	unsigned int sliderContextQuad = gl::GUI::createQuad(-0.495f, 0.1f, 0.2f, 0.05f);
	unsigned int slideQuad = gl::GUI::createQuad(-0.495f, 0.1f, 0.02f, 0.05f);
	unsigned int pullButtonQuad = gl::GUI::createQuad(-0.495f, 0.19f, 0.29f, 0.05f);
        unsigned int windowQuad = gl::GUI::createQuad(-0.5f, 0.2f, 0.3f, 0.3f);
	//GUI Lines
	unsigned int line = gl::GUI::createLine(glm::vec2(-0.495f, 0.075f), glm::vec2(-0.205f, 0.075f), 8);

        gl::GUI::colorQuad(quitButtonQuad, 7);
        gl::GUI::colorQuad(playButtonQuad, 7);
        gl::GUI::colorQuad(windowQuad, 7);
        gl::GUI::colorQuad(pullButtonQuad, 6);
	gl::GUI::colorQuad(slideQuad, 6);
	
	gl::GUI::bindQuadParent(windowQuad, pullButtonQuad);
	gl::GUI::bindQuadParent(windowQuad, sliderContextQuad);
	gl::GUI::bindQuadParent(sliderContextQuad, slideQuad);
	//gl::GUI::bindLineParent(sliderContextQuad, line);
        unsigned quit_button = gl::GUI::addButton(quitButtonQuad);
        unsigned pull_button = gl::GUI::addButton(pullButtonQuad);
	unsigned slider_button = gl::GUI::addButton(slideQuad);
        //Signals
	
        reserve_signals<KeyEvent>(9);
	unsigned int esc_press = create_signal(KeyEvent(GLFW_KEY_ESCAPE, KeyCondition(1, 0)));
        unsigned int c_press = create_signal(KeyEvent(GLFW_KEY_C, KeyCondition(1, 0)));
        unsigned int g_presst = create_signal(KeyEvent(GLFW_KEY_G, KeyCondition(1, 0)));
        unsigned int h_press = create_signal(KeyEvent(GLFW_KEY_H, KeyCondition(1, 0)));
        unsigned int i_press = create_signal(KeyEvent(GLFW_KEY_I, KeyCondition(1, 0)));
	unsigned int w_press = create_signal(KeyEvent(GLFW_KEY_W, KeyCondition(1, 0)));
	unsigned int w_release = create_signal(KeyEvent(GLFW_KEY_W, KeyCondition(0, 0)));
	unsigned int s_press = create_signal(KeyEvent(GLFW_KEY_S, KeyCondition(1, 0)));
	unsigned int s_release = create_signal(KeyEvent(GLFW_KEY_S, KeyCondition(0, 0)));
	unsigned int a_press = create_signal(KeyEvent(GLFW_KEY_A, KeyCondition(1, 0)));
	unsigned int a_release = create_signal(KeyEvent(GLFW_KEY_A, KeyCondition(0, 0)));
	unsigned int d_press = create_signal(KeyEvent(GLFW_KEY_D, KeyCondition(1, 0)));
	unsigned int d_release = create_signal(KeyEvent(GLFW_KEY_D, KeyCondition(0, 0)));
	unsigned int space_press = create_signal(KeyEvent(GLFW_KEY_SPACE, KeyCondition(1, 0)));
	unsigned int space_release = create_signal(KeyEvent(GLFW_KEY_SPACE, KeyCondition(0, 0)));
	unsigned int z_press = create_signal(KeyEvent(GLFW_KEY_Z, KeyCondition(1, 0)));
	unsigned int z_release = create_signal(KeyEvent(GLFW_KEY_Z, KeyCondition(0, 0)));
	reserve_signals<MouseKeyEvent>(4);
	unsigned int rmb_press = create_signal(MouseKeyEvent(1, KeyCondition(1, 0)));
	unsigned int rmb_release = create_signal(MouseKeyEvent(1, KeyCondition(0, 0)));
	unsigned int lmb_press = create_signal(MouseKeyEvent(0, KeyCondition(1, 0)));
	unsigned int lmb_release = create_signal(MouseKeyEvent(0, KeyCondition(0, 0)));
	reserve_signals<MouseEvent>(3);
	unsigned int quit_button_press = create_signal(MouseEvent(quit_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	unsigned int pull_button_press = create_signal(MouseEvent(pull_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	unsigned int slider_button_press = create_signal(MouseEvent(slider_button, MouseKeyEvent(0, KeyCondition(1, 0))));

	Func<void, unsigned int> slideFollow = create_func(gl::GUI::moveQuadByMouseDelta, slideQuad);
	Func<void, Func<void, unsigned int>> slideStartFollow = create_func(func_start_rule<void, unsigned int>, slideFollow);
	Func<void, Func<void, unsigned int>> slideStopFollow = create_func(func_stop_rule<void, unsigned int>, slideFollow);
	slideStartFollow.listen({ slider_button_press });
	slideStopFollow.listen({ lmb_release });
	
	
	Func<void, unsigned int> windowFollow = create_func(gl::GUI::moveQuadByMouseDelta, windowQuad);
	Func<void, Func<void, unsigned int>> windowStartFollow = create_func(func_start_rule<void, unsigned int>, windowFollow);
	Func<void, Func<void, unsigned int>> windowStopFollow = create_func(func_stop_rule<void, unsigned int>, windowFollow);
	windowStartFollow.listen({ pull_button_press });
	windowStopFollow.listen({ lmb_release });
	
	Func<void> quitFunc = create_func(quit);
	quitFunc.listen({ esc_press, quit_button_press });
	Func<void> trackMouseFunc = create_func(toggleTrackMouse);
	trackMouseFunc.listen({ c_press, rmb_press, rmb_release });

	Func<void> forwardFunc = create_func(gl::Camera::forward);
	Func<void> backFunc = create_func(gl::Camera::back);
	Func<void> leftFunc = create_func(gl::Camera::left);
	Func<void> rightFunc = create_func(gl::Camera::right);
	Func<void> upFunc = create_func(gl::Camera::up);
	Func<void> downFunc = create_func(gl::Camera::down);
	
	Func<void, Func<void>> startForwardFunc = create_func(func_start_rule<void>, forwardFunc);
	startForwardFunc.listen({ w_press });
	Func<void, Func<void>> stopForwardFunc = create_func(func_stop_rule<void>, forwardFunc);
	stopForwardFunc.listen({ w_release });

	Func<void, Func<void>> startBackFunc = create_func(func_start_rule<void>, backFunc);
	startBackFunc.listen({ s_press });
	Func<void, Func<void>> stopBackFunc = create_func(func_stop_rule<void>, backFunc);
	stopBackFunc.listen({ s_release });

	Func<void, Func<void>> startLeftFunc = create_func(func_start_rule<void>, leftFunc);
	startLeftFunc.listen({ a_press });
	Func<void, Func<void>> stopLeftFunc = create_func(func_stop_rule<void>, leftFunc);
	stopLeftFunc.listen({ a_release });

	Func<void, Func<void>> startRightFunc = create_func(func_start_rule<void>, rightFunc);
	startRightFunc.listen({ d_press });
	Func<void, Func<void>> stopRightFunc = create_func(func_stop_rule<void>, rightFunc);
	stopRightFunc.listen({ d_release });

	Func<void, Func<void>> startUpFunc = create_func(func_start_rule<void>, upFunc);
	startUpFunc.listen({ space_press });
	Func<void, Func<void>> stopUpFunc = create_func(func_stop_rule<void>, upFunc);
	stopUpFunc.listen({ space_release });

	Func<void, Func<void>> startDownFunc = create_func(func_start_rule<void>, downFunc);
	startDownFunc.listen({ z_press });
	Func<void, Func<void>> stopDownFunc = create_func(func_stop_rule<void>, downFunc);
	stopDownFunc.listen({ z_release });
	
        //Text
        gl::GUI::Text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
        unsigned int pl_tb = gl::GUI::Text::createTextbox(playButtonQuad, 0, 0);
        unsigned int qu_tb = gl::GUI::Text::createTextbox(quitButtonQuad, 0, 0);
        
        gl::GUI::Text::setTextboxString(qu_tb, "QUIT");
        gl::GUI::Text::setTextboxString(pl_tb, "Play");
        gl::GUI::Text::loadTextboxes();

        //Lights
        gl::Lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.95f, 0.1f, 10.0f));
        gl::Lighting::createLight(glm::vec4(3.0f, 5.0f, 3.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.2f, 10.0f));

        
        while (state == App::MainMenu) {

                light_pos += light_mov;
                gl::Lighting::setLightPos(1, light_pos);
                gl::Camera::look(Input::cursorFrameDelta);
                gl::Camera::update();
                gl::updateGeneralUniformBuffer();
                gl::Models::updateBuffers();
                gl::Lighting::updateLightIndexRangeBuffer();
                gl::Lighting::updateLightDataBuffer();
                gl::GUI::Text::updateCharStorage();
                gl::GUI::updateColoredQuads();
                gl::GUI::updateQuadBuffer();
		gl::GUI::updateLines();
		gl::GUI::updateElements();
                glBindFramebuffer(GL_FRAMEBUFFER, gl::Texture::gBuffer);
                glViewport(0, 0, gl::screenWidth*gl::resolution, gl::screenHeight*gl::resolution);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                gl::Models::render();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(0, 0, gl::screenWidth, gl::screenHeight);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                gl::Lighting::renderLights();
                gl::Debug::drawGrid();
                //GUI
                glDepthFunc(GL_ALWAYS);
                gl::GUI::renderColoredQuads();
		gl::GUI::renderLines();
                gl::GUI::Text::renderGlyphs();
                glDepthFunc(GL_LESS);
                
                glfwSwapBuffers(mainWindow.window);
                fetchInput();
		
		Debug::printErrors();
		updateTime();
		updateTimeFactor();
		//limitFPS();

		Debug::printInfo();
	}
	
}

void App::fetchInput()
{
        gl::GUI::updateButtonBuffer();
        gl::GUI::rasterButtons();
        Input::fetchGLFWEvents();
        gl::GUI::fetchButtonMap();
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
