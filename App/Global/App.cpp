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
#include <functional>
#include <algorithm>
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
	using namespace gl::GUI;
	//Buttons

	size_t quitButtonQuad = gl::GUI::createQuad(-1.0f, -0.9f, 0.2f, 0.07f);
	size_t playButtonQuad = gl::GUI::createQuad(-1.0f, -0.7f, 0.2f, 0.07f);
	
	size_t windowQuad = gl::GUI::createQuad(-0.5f, 0.2f, 0.2f, 0.2f);
	size_t pullQuad = gl::GUI::createQuad(-0.5f, 0.2f, 0.2f, 0.04f);
	size_t boundaryQuad = gl::GUI::createQuad(-0.8f, 0.8f, 0.6f, 0.6f);
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

	//move quad
	using OffsetQuadOperation = Operation<glm::vec2, Set, Pass<glm::vec2>, Add<glm::vec2>>;
	OffsetQuadOperation offsetPullQuad(getQuad(boundaryQuad).size, Add<glm::vec2>(getQuad(boundaryQuad).size, cursorFrameDelta));
	OffsetQuadOperation offsetWindowQuad(getQuad(boundaryQuad).size, Add<glm::vec2>(getQuad(boundaryQuad).size, cursorFrameDelta));

	Func<void> movePullQuadFunc(offsetPullQuad);
	Func<void> moveWindowQuadFunc(offsetWindowQuad);

	Func<void, Func<void>> startMovePullQuad(func_start_rule<void>, movePullQuadFunc);
	startMovePullQuad.listen({ pull_button_press });

	Func<void, Func<void>> stopMovePullQuad(func_stop_rule<void>, movePullQuadFunc);
	stopMovePullQuad.listen({ pull_button_release, lmb_release });

	Func<void, Func<void>> startMoveWindowQuad(func_start_rule<void>, moveWindowQuadFunc);
	startMoveWindowQuad.listen({ pull_button_press });

	Func<void, Func<void>> stopMoveWindowQuad(func_stop_rule<void>, moveWindowQuadFunc);
	stopMoveWindowQuad.listen({ pull_button_release, lmb_release });


	//quad bounds
	using AddFloatOperation = Operation<float, Add, Pass<float>, Pass<float>>;
	using SubstractFloatOperation = Operation<float, Substract, Pass<float>, Pass<float>>;
	using MaxFloatOperation = Operation<float, Max, Pass<float>, Pass<float>>;
	using MinFloatOperation = Operation<float, Min, Pass<float>, Pass<float>>;
	using BoundFloatMaxOperation = Operation<float, Max, Pass<float>, Operation<float, Add, Substract<float>, Pass<float>>>;
	using BoundFloatMinOperation = Operation<float, Min, Pass<float>, Operation<float, Substract, Add<float>, Pass<float>>>;
	Operation<float, Set, Pass<float>, MaxFloatOperation> boundPullQuadMinX(getQuad(pullQuad).pos.x, MaxFloatOperation(getQuad(pullQuad).pos.x, getQuad(boundaryQuad).pos.x));
	Operation<float, Set, Pass<float>, MaxFloatOperation> boundWindowQuadMinX(getQuad(windowQuad).pos.x, MaxFloatOperation(getQuad(windowQuad).pos.x, getQuad(boundaryQuad).pos.x));

	Func<void> pullMaxBoundXMinFunc(boundPullQuadMinX);
	func_start_rule(pullMaxBoundXMinFunc);
	Func<void> pullMinBoundXMinFunc(boundPullQuadMinX);
	func_start_rule(pullMinBoundXMinFunc);
	Func<void> windowMaxBoundXMinFunc(boundWindowQuadMinX);
	func_start_rule(windowMaxBoundXMinFunc);
	Func<void> windowMinBoundXMinFunc(boundWindowQuadMinX);
	func_start_rule(windowMinBoundXMinFunc);

	Operation<float, Set, Pass<float>, BoundFloatMinOperation> 
		boundPullQuadMaxX(getQuad(pullQuad).pos.x, BoundFloatMinOperation(getQuad(pullQuad).pos.x, Operation<float, Substract, Add<float>, Pass<float>>(Add<float>(getQuad(boundaryQuad).pos.x, getQuad(boundaryQuad).size.x), getQuad(pullQuad).size.x)));
	Operation<float, Set, Pass<float>, BoundFloatMinOperation>
		boundWindowQuadMaxX(getQuad(windowQuad).pos.x, BoundFloatMinOperation(getQuad(windowQuad).pos.x, Operation<float, Substract, Add<float>, Pass<float>>(Add<float>(getQuad(boundaryQuad).pos.x, getQuad(boundaryQuad).size.x), getQuad(windowQuad).size.x)));

	Func<void> pullMaxBoundXMaxFunc(boundPullQuadMaxX);
	func_start_rule(pullMaxBoundXMaxFunc);
	Func<void> pullMinBoundXMaxFunc(boundPullQuadMaxX);
	func_start_rule(pullMinBoundXMaxFunc);
	
	Func<void> windowMaxBoundXMaxFunc(boundWindowQuadMaxX);
	func_start_rule(windowMaxBoundXMaxFunc);
	Func<void> windowMinBoundXMaxFunc(boundWindowQuadMaxX);
	func_start_rule(windowMinBoundXMaxFunc);

Operation<float, Set, Pass<float>, BoundFloatMaxOperation>
		boundPullQuadMaxY(getQuad(pullQuad).pos.y, BoundFloatMaxOperation(getQuad(pullQuad).pos.y, Operation<float, Add, Substract<float>, Pass<float>>(Substract<float>(getQuad(boundaryQuad).pos.y, getQuad(boundaryQuad).size.y), getQuad(windowQuad).size.y)));
	Operation<float, Set, Pass<float>, BoundFloatMaxOperation>
		boundWindowQuadMaxY(getQuad(windowQuad).pos.y, BoundFloatMaxOperation(getQuad(windowQuad).pos.y, Operation<float, Add, Substract<float>, Pass<float>>(Substract<float>(getQuad(boundaryQuad).pos.y, getQuad(boundaryQuad).size.y), getQuad(windowQuad).size.y)));
	
	Func<void> pullMaxBoundYMaxFunc(boundPullQuadMaxY);
	func_start_rule(pullMaxBoundYMaxFunc);
	Func<void> pullMinBoundYMaxFunc(boundPullQuadMaxY);
	func_start_rule(pullMinBoundYMaxFunc);
	
	Func<void> windowMaxBoundYMaxFunc(boundWindowQuadMaxY);
	func_start_rule(windowMaxBoundYMaxFunc);
	Func<void> windowMinBoundYMaxFunc(boundWindowQuadMaxY);
	func_start_rule(windowMinBoundYMaxFunc);

	Operation<float, Set, Pass<float>, MinFloatOperation> boundPullQuadMinY(getQuad(pullQuad).pos.y, MinFloatOperation(getQuad(pullQuad).pos.y, getQuad(boundaryQuad).pos.y));
	Operation<float, Set, Pass<float>, MinFloatOperation> boundWindowQuadMinY(getQuad(windowQuad).pos.y, MinFloatOperation(getQuad(windowQuad).pos.y, getQuad(boundaryQuad).pos.y));

	Func<void> pullMaxBoundYMinFunc(boundPullQuadMinY);
	func_start_rule(pullMaxBoundYMinFunc);
	Func<void> pullMinBoundYMinFunc(boundPullQuadMinY);
	func_start_rule(pullMinBoundYMinFunc);
	Func<void> windowMaxBoundYMinFunc(boundWindowQuadMinY);
	func_start_rule(windowMaxBoundYMinFunc);
	Func<void> windowMinBoundYMinFunc(boundWindowQuadMinY);
	func_start_rule(windowMinBoundYMinFunc);

	


	//general functions
	Func<void> quitFunc(quit);
	quitFunc.listen({ esc_press, quit_button_press });
	Func<void> hideCursorFunc(App::Input::toggleCursor);
	hideCursorFunc.listen({ c_press, rmb_press, rmb_release });
	Func<void> trackMouseFunc(gl::Camera::toggleLook);
	trackMouseFunc.listen({ c_press, rmb_press, rmb_release });

	Func<void> forwardFunc(gl::Camera::forward);
	Func<void> backFunc(gl::Camera::back);
	Func<void> leftFunc(gl::Camera::left);
	Func<void> rightFunc(gl::Camera::right);
	Func<void> upFunc(gl::Camera::up);
	Func<void> downFunc(gl::Camera::down);

	Func<void, Func<void>> startForwardFunc(func_start_rule<void>, forwardFunc);
	startForwardFunc.listen({ w_press });
	Func<void, Func<void>> stopForwardFunc(func_stop_rule<void>, forwardFunc);
	stopForwardFunc.listen({ w_release });

	Func<void, Func<void>> startBackFunc(func_start_rule<void>, backFunc);
	startBackFunc.listen({ s_press });
	Func<void, Func<void>> stopBackFunc(func_stop_rule<void>, backFunc);
	stopBackFunc.listen({ s_release });

	Func<void, Func<void>> startLeftFunc(func_start_rule<void>, leftFunc);
	startLeftFunc.listen({ a_press });
	Func<void, Func<void>> stopLeftFunc(func_stop_rule<void>, leftFunc);
	stopLeftFunc.listen({ a_release });

	Func<void, Func<void>> startRightFunc(func_start_rule<void>, rightFunc);
	startRightFunc.listen({ d_press });
	Func<void, Func<void>> stopRightFunc(func_stop_rule<void>, rightFunc);
	stopRightFunc.listen({ d_release });

	Func<void, Func<void>> startUpFunc(func_start_rule<void>, upFunc);
	startUpFunc.listen({ space_press });
	Func<void, Func<void>> stopUpFunc(func_stop_rule<void>, upFunc);
	stopUpFunc.listen({ space_release });

	Func<void, Func<void>> startDownFunc(func_start_rule<void>, downFunc);
	startDownFunc.listen({ z_press });
	Func<void, Func<void>> stopDownFunc(func_stop_rule<void>, downFunc);
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
