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
#include <OpenGL\GUI\UI\Quad.h>
#include <OpenGL\GUI\UI\Line.h>
#include <OpenGL\GUI\UI\Delta.h>
#include <OpenGL\GUI\UI\Boundary.h>
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
	size_t glfw = glfwInit();
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

	size_t quitButtonQuad = gl::GUI::createQuad(-1.0f, -0.9f, 0.2f, 0.07f);
	size_t playButtonQuad = gl::GUI::createQuad(-1.0f, -0.7f, 0.2f, 0.07f);
	
	size_t windowQuad = gl::GUI::createQuad(-0.5f, 0.2f, 0.2f, 0.2f);
	size_t pullQuad = gl::GUI::createQuad(-0.5f, 0.2f, 0.2f, 0.04f);
	size_t boundaryQuad = gl::GUI::createQuad(-0.8, 0.8, 0.6, 0.6);
	//GUI Lines
	//size_t line = gl::GUI::createLine(glm::vec2(-0.495f, 0.075f), glm::vec2(-0.205f, 0.075f), 8);
	
	gl::GUI::colorQuad(quitButtonQuad, 7);
	gl::GUI::colorQuad(playButtonQuad, 7);
	gl::GUI::colorQuad(boundaryQuad, 4);
	gl::GUI::colorQuad(windowQuad, 1);
	gl::GUI::colorQuad(pullQuad, 6);

	size_t quit_button = gl::GUI::addButtonQuad(quitButtonQuad);
	size_t pull_button = gl::GUI::addButtonQuad(pullQuad);
	//deltas

	size_t cursorDeltaSource = gl::GUI::createSource(App::Input::cursorFrameDelta);
	size_t pullDeltaTarget = gl::GUI::createTarget<glm::vec2>(gl::GUI::getQuad(pullQuad).pos, cursorDeltaSource);
	size_t windowDeltaTarget = gl::GUI::createTarget<glm::vec2>(gl::GUI::getQuad(windowQuad).pos, cursorDeltaSource);
	gl::GUI::createBoundary(gl::GUI::getQuad(pullQuad).pos.x, gl::GUI::getQuad(boundaryQuad).pos.x);
	gl::GUI::createBoundary(gl::GUI::getQuad(windowQuad).pos.x, gl::GUI::getQuad(boundaryQuad).pos.x);
	gl::GUI::createBoundary<float, gl::GUI::MinPolicy>(gl::GUI::getQuad(pullQuad).pos.y, gl::GUI::getQuad(boundaryQuad).pos.y);
	gl::GUI::createBoundary<float, gl::GUI::MinPolicy>(gl::GUI::getQuad(windowQuad).pos.y, gl::GUI::getQuad(boundaryQuad).pos.y);
	//Signals

	reserve_signals<KeyEvent>(9);
	size_t esc_press = create_signal(KeyEvent(GLFW_KEY_ESCAPE, KeyCondition(1, 0)));
	size_t c_press = create_signal(KeyEvent(GLFW_KEY_C, KeyCondition(1, 0)));
	size_t g_presst = create_signal(KeyEvent(GLFW_KEY_G, KeyCondition(1, 0)));
	size_t h_press = create_signal(KeyEvent(GLFW_KEY_H, KeyCondition(1, 0)));
	size_t i_press = create_signal(KeyEvent(GLFW_KEY_I, KeyCondition(1, 0)));
	size_t w_press = create_signal(KeyEvent(GLFW_KEY_W, KeyCondition(1, 0)));
	size_t w_release = create_signal(KeyEvent(GLFW_KEY_W, KeyCondition(0, 0)));
	size_t s_press = create_signal(KeyEvent(GLFW_KEY_S, KeyCondition(1, 0)));
	size_t s_release = create_signal(KeyEvent(GLFW_KEY_S, KeyCondition(0, 0)));
	size_t a_press = create_signal(KeyEvent(GLFW_KEY_A, KeyCondition(1, 0)));
	size_t a_release = create_signal(KeyEvent(GLFW_KEY_A, KeyCondition(0, 0)));
	size_t d_press = create_signal(KeyEvent(GLFW_KEY_D, KeyCondition(1, 0)));
	size_t d_release = create_signal(KeyEvent(GLFW_KEY_D, KeyCondition(0, 0)));
	size_t space_press = create_signal(KeyEvent(GLFW_KEY_SPACE, KeyCondition(1, 0)));
	size_t space_release = create_signal(KeyEvent(GLFW_KEY_SPACE, KeyCondition(0, 0)));
	size_t z_press = create_signal(KeyEvent(GLFW_KEY_Z, KeyCondition(1, 0)));
	size_t z_release = create_signal(KeyEvent(GLFW_KEY_Z, KeyCondition(0, 0)));

	reserve_signals<MouseKeyEvent>(4);
	size_t rmb_press = create_signal(MouseKeyEvent(1, KeyCondition(1, 0)));
	size_t rmb_release = create_signal(MouseKeyEvent(1, KeyCondition(0, 0)));
	size_t lmb_press = create_signal(MouseKeyEvent(0, KeyCondition(1, 0)));
	size_t lmb_release = create_signal(MouseKeyEvent(0, KeyCondition(0, 0)));

	reserve_signals<MouseEvent>(4);
	reserve_signals<CursorEvent>(2);
	size_t quit_button_press = create_signal(MouseEvent(quit_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	
	size_t pull_button_press = create_signal(MouseEvent(pull_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	size_t pull_button_release = create_signal(MouseEvent(pull_button, MouseKeyEvent(0, KeyCondition(0, 0))));
	size_t pull_button_leave = create_signal(CursorEvent(pull_button, 0));
	size_t pull_button_enter = create_signal(CursorEvent(pull_button, 1));
	Func<void, size_t> pullFollowFunc = create_func(gl::GUI::activateTarget<glm::vec2>, pullDeltaTarget);
	pullFollowFunc.listen({ pull_button_press });
	Func<void, size_t> pullStopFollowFunc = create_func(gl::GUI::deactivateTarget<glm::vec2>, pullDeltaTarget);
	pullStopFollowFunc.listen({ pull_button_release, pull_button_leave });
	Func<void, size_t> windowFollowFunc = create_func(gl::GUI::activateTarget<glm::vec2>, windowDeltaTarget);
	windowFollowFunc.listen({ pull_button_press});
	Func<void, size_t> windowStopFollowFunc = create_func(gl::GUI::deactivateTarget<glm::vec2>, windowDeltaTarget);
	windowStopFollowFunc.listen({ pull_button_release, pull_button_leave });

	Func<void> quitFunc = create_func(quit);
	quitFunc.listen({ esc_press, quit_button_press });
	Func<void> hideCursorFunc = create_func(App::Input::toggleCursor);
	hideCursorFunc.listen({ c_press, rmb_press, rmb_release });
	Func<void> trackMouseFunc = create_func(gl::Camera::toggleLook);
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
	size_t pl_tb = gl::GUI::Text::createTextbox(playButtonQuad, 0, 0);
	size_t qu_tb = gl::GUI::Text::createTextbox(quitButtonQuad, 0, 0);

	gl::GUI::Text::setTextboxString(qu_tb, "QUIT");
	gl::GUI::Text::setTextboxString(pl_tb, "Play");
	gl::GUI::Text::loadTextboxes();

	//Lights
	gl::Lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.95f, 0.1f, 10.0f));
	gl::Lighting::createLight(glm::vec4(3.0f, 5.0f, 3.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.2f, 10.0f));


	while (state == App::MainMenu) {

		light_pos += light_mov;
		gl::Lighting::setLightPos(1, light_pos);
		
		gl::Camera::update();
		gl::GUI::updateDeltas<glm::vec2>();
		gl::GUI::updateBoundaries<float>();
		gl::GUI::updateBoundaries<float, gl::GUI::MinPolicy>();
		gl::GUI::updateLines();

		gl::updateGeneralUniformBuffer();
		gl::Models::updateBuffers();
		gl::Lighting::updateLightIndexRangeBuffer();
		gl::Lighting::updateLightDataBuffer();
		gl::GUI::Text::updateCharStorage();
		gl::GUI::updateColoredQuads();
		gl::GUI::updateQuadBuffer();

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
	gl::Camera::look(Input::cursorFrameDelta);
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

void App::setTargetFPS(size_t pTargetFPS)
{
	targetFrameMS = (size_t)(1000.0f / (float)pTargetFPS);
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
