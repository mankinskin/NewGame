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
	float window_edge_marging = 0.005f;
	float window_width = 0.2f;
	float window_height = 0.2f;
	size_t quitButtonQuad = createQuad(-1.0f, -0.9f, 0.2f, 0.07f);
	size_t playButtonQuad = createQuad(-1.0f, -0.7f, 0.2f, 0.07f);
	
	size_t windowQuad = createQuad(-0.5f, 0.2f, window_width, window_height);
	size_t headQuad = createQuad(-0.5f, 0.2f, 0.2f, 0.04f);

	float slider_height = 0.05f;
	float slider_width = window_width - window_edge_marging*2.0f;
	float slider_slide_width = 0.01f;
	
	size_t sliderBoundQuad = createQuad(-0.5f + window_edge_marging, 0.12f, slider_width, slider_height);
	size_t sliderSlideQuad = createQuad(-0.5f + window_edge_marging, 0.12f, slider_slide_width, slider_height);

	//GUI Lines
	//size_t line = gl::GUI::createLine(glm::vec2(-0.495f, 0.075f), glm::vec2(-0.205f, 0.075f), 8);
	
	colorQuad(quitButtonQuad, 7);
	colorQuad(playButtonQuad, 7);
	colorQuad(windowQuad, 1);
	colorQuad(headQuad, 6);

	colorQuad(sliderBoundQuad, 6);
	colorQuad(sliderSlideQuad, 7);

	size_t quit_button = addButtonQuad(quitButtonQuad);
	size_t head_button = addButtonQuad(headQuad);

	size_t slider_button = addButtonQuad(sliderBoundQuad);
	
	//Signals
	reserve_signals<KeyEvent>(17);
	size_t esc_press = create_signal(KeyEvent(GLFW_KEY_ESCAPE, KeyCondition(1, 0)));
	size_t c_press = create_signal(KeyEvent(GLFW_KEY_C, KeyCondition(1, 0)));
	size_t g_press = create_signal(KeyEvent(GLFW_KEY_G, KeyCondition(1, 0)));
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

	reserve_signals<MouseEvent>(6);
	size_t quit_button_press = create_signal(MouseEvent(quit_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	size_t pull_button_press = create_signal(MouseEvent(head_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	size_t pull_button_release = create_signal(MouseEvent(head_button, MouseKeyEvent(0, KeyCondition(0, 0))));
	size_t slider_button_press = create_signal(MouseEvent(slider_button, MouseKeyEvent(0, KeyCondition(1, 0))));
	size_t slider_button_release = create_signal(MouseEvent(slider_button, MouseKeyEvent(0, KeyCondition(0, 0))));
	
	reserve_signals<CursorEvent>(2);
	size_t pull_button_leave = create_signal(CursorEvent(head_button, 0));
	size_t pull_button_enter = create_signal(CursorEvent(head_button, 1));
	
	void(*offsetFloat)(float&, float&) = [](float& pVal, float& pDelta) { pVal += pDelta; };
	void(*setFloatToScale)(float&, float&, float&) = [](float& pVal, float& pRange, float& pPos) { pVal = (pPos)/ pRange * 100.0f; };
	void(*moveQuad)(glm::vec2&, glm::vec2&) = [](glm::vec2& pQuadPos, glm::vec2& pDelta) { pQuadPos += pDelta; };
	Functor<void, glm::vec2&, glm::vec2&> movePullQuadFunc(moveQuad, getQuad(headQuad).pos, cursorFrameDelta);
	setup_functor_rule<void, glm::vec2&, glm::vec2&>(movePullQuadFunc, { pull_button_press }, { lmb_release });

	Functor<void, glm::vec2&, glm::vec2&> moveWindowQuadFunc(moveQuad, getQuad(windowQuad).pos, cursorFrameDelta);
	setup_functor_rule<void, glm::vec2&, glm::vec2&>(moveWindowQuadFunc, { pull_button_press }, { lmb_release });


	Functor<void, glm::vec2&, glm::vec2&> moveSliderBoundQuadFunc(moveQuad, getQuad(sliderBoundQuad).pos, cursorFrameDelta);
	setup_functor_rule<void, glm::vec2&, glm::vec2&>(moveSliderBoundQuadFunc, { pull_button_press }, { lmb_release });

	Functor<void, glm::vec2&, glm::vec2&> moveSliderQuadFunc(moveQuad, getQuad(sliderSlideQuad).pos, cursorFrameDelta);
	setup_functor_rule<void, glm::vec2&, glm::vec2&>(moveSliderQuadFunc, { pull_button_press }, { lmb_release });
	void(*setNewSlidePos)(float&, float&, float&) = [](float& pVal, float& pNew, float& pNewSub) { pVal = pNew - pNewSub; };
	Functor<void, float&, float&, float&> setSlideQuadFunc(setNewSlidePos, getQuad(sliderSlideQuad).size.x, relativeCursorPosition.x, getQuad(sliderBoundQuad).pos.x, { slider_button_press });
	Functor<void, float&, float&> moveSlideQuadFunc(offsetFloat, getQuad(sliderSlideQuad).size.x, cursorFrameDelta.x);
	setup_functor_rule<void, float&, float&>(moveSlideQuadFunc, { slider_button_press }, { lmb_release });

	//Lights
	gl::Lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.95f, 0.1f, 10.0f));
	gl::Lighting::createLight(glm::vec4(3.0f, 5.0f, 3.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.2f, 10.0f));
	
	
	
	void(*maxFloat_noref)(float&, float) = [](float& pTarget, float pSource) { pTarget = pTarget > pSource ? pTarget : pSource; };
	void(*maxFloat)(float&, float&) = [](float& pTarget, float& pSource) { pTarget = pTarget > pSource ? pTarget : pSource; };
	void(*minFloat)(float&, float&) = [](float& pTarget, float& pSource) { pTarget = pTarget < pSource ? pTarget : pSource; };
	void(*maxFloatBound)(float&, float&, float&, float&) =
		[](float& pTarget, float& pTargetAdd, float& pSource, float& pSourceAdd) { pTarget = pTarget > ((pSource - pSourceAdd) + pTargetAdd) ? pTarget : ((pSource - pSourceAdd) + pTargetAdd); };
	void(*minFloatBound)(float&, float&, float&, float&) = 
		[](float& pTarget, float& pTargetAdd, float& pSource, float& pSourceAdd) { pTarget = pTarget < ((pSource + pSourceAdd)-pTargetAdd) ? pTarget : ((pSource + pSourceAdd) - pTargetAdd); };
	
	make_default_rule_functor(Functor<void, float&, float&>(minFloat, getQuad(sliderSlideQuad).size.x, getQuad(sliderBoundQuad).size.x));
	make_default_rule_functor(Functor<void, float&, float>(maxFloat_noref, getQuad(sliderSlideQuad).size.x, 0.0f));
	make_default_rule_functor(Functor<void, float&, float&, float&>(setFloatToScale, gl::Lighting::getLightColor(1).w, getQuad(sliderBoundQuad).size.x, getQuad(sliderSlideQuad).size.x));

	//general functions
	Functor<void> quitFunc(quit, { esc_press, quit_button_press });
	Functor<void> hideCursorFunc(App::Input::toggleCursor, { c_press, rmb_press, rmb_release });
	Functor<void> trackMouseFunc(gl::Camera::toggleLook, { c_press, rmb_press, rmb_release });
	Functor<void> (gl::Debug::toggleGrid, { g_press });
	Functor<void> (gl::Debug::toggleCoord, { h_press });

	Functor<void> forwardFunc(gl::Camera::forward);


	setup_functor_rule(forwardFunc, { w_press }, { w_release });
	Functor<void> backFunc(gl::Camera::back);
	setup_functor_rule(backFunc, { s_press }, { s_release });
	Functor<void> leftFunc(gl::Camera::left);
	setup_functor_rule(leftFunc, { a_press }, { a_release });
	Functor<void> rightFunc(gl::Camera::right);
	setup_functor_rule(rightFunc, { d_press }, { d_release });
	Functor<void> upFunc(gl::Camera::up);
	setup_functor_rule(upFunc, { space_press }, { space_release });
	Functor<void> downFunc(gl::Camera::down);
	setup_functor_rule(downFunc, { z_press }, { z_release });


	//Text
	gl::GUI::Text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
	size_t pl_tb = gl::GUI::Text::createTextbox(playButtonQuad, 0, 0);
	size_t qu_tb = gl::GUI::Text::createTextbox(quitButtonQuad, 0, 0);

	gl::GUI::Text::setTextboxString(qu_tb, "QUIT");
	gl::GUI::Text::setTextboxString(pl_tb, "Play");
	gl::GUI::Text::loadTextboxes();

	


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
	gl::GUI::Text::clearCharStorage();
	
	gl::GUI::clearBuffers();
	gl::GUI::clearButtons();
	Input::clearFunctors();
	Input::clearSignals();
}

void App::fetchInput()
{
	
	gl::GUI::updateButtonBuffer();
	gl::GUI::rasterButtons();
	Input::fetchGLFWEvents();
	gl::GUI::fetchButtonMap();
	Input::checkEvents();
	Input::callFunctors();
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
