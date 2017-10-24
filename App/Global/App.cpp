#include "stdafx.h"
#include "App.h"
#include "Debug.h"
#include "..\Input\Input.h"
#include "..\Input\Keys.h"
#include "..\Input\Functor.h"
#include "..\Input\Mouse.h"
#include "..\Input\Event.h"
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
#include <OpenGL\GUI\UI\Element.h>
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
	
	size_t quitButtonQuad = createQuad(-1.0f, -0.9f, 0.2f, 0.07f);
	size_t playButtonQuad = createQuad(-1.0f, -0.7f, 0.2f, 0.07f);

	void(*moveQuad)(glm::vec2&, glm::vec2&) = [](glm::vec2& pQuadPos, glm::vec2& pDelta) { pQuadPos += pDelta; };

	
	//-----------FANCY QUAD---------------------------------------------------------------------------------
	///float total_width = 0.5f;
	///float border_width = total_width * 0.02f;
	///float border_height = border_width * (1600.0f/850.0f);
	///
	///
	///
	///
	///size_t quads[9] = { createQuad(0.0f, 0.0f, border_width, border_height),
	///					createQuad(border_width, 0.0f, total_width - (border_width*2.0f), border_height),
	///					createQuad(total_width - border_width, 0.0f, border_width, border_height),
	///					
	///					createQuad(0.0f, -border_height, border_width, total_width - (border_height*2.0f)),
	///					createQuad(border_width, -border_height, total_width - (border_width*2.0f), total_width - (border_height*2.0f)),
	///					createQuad(total_width - border_width, -border_height, border_width, total_width - (border_height*2.0f)),
	///					
	///					createQuad(0.0f, -total_width + border_height , border_width, border_height),
	///					createQuad(border_width, -total_width + border_height, total_width - (border_width*2.0f), border_height),
	///					createQuad(total_width - border_width, -total_width + border_height, border_width, border_height) };
	///QuadConstruct<BorderCornerQuads> fancyQuad(BorderCornerQuads(quads));
	///colorQuad(quads[0], TextureColor(createAtlasUVRange(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)), 1));
	///colorQuad(quads[1], TextureColor(createAtlasUVRange(glm::vec4(0.5f, 0.0f, 0.5f, 0.5f)), 1));
	///colorQuad(quads[2], TextureColor(createAtlasUVRange(glm::vec4(0.5f, 0.0f, 1.0f, 0.5f)), 1));
	///colorQuad(quads[3], TextureColor(createAtlasUVRange(glm::vec4(0.0f, 0.5f, 0.5f, 0.5f)), 1));
	///colorQuad(quads[4], TextureColor(createAtlasUVRange(glm::vec4(0.5f, 0.5f, 0.5f, 0.5f)), 1));
	///colorQuad(quads[5], TextureColor(createAtlasUVRange(glm::vec4(0.5f, 0.5f, 1.0f, 0.5f)), 1));
	///colorQuad(quads[6], TextureColor(createAtlasUVRange(glm::vec4(0.0f, 0.5f, 0.5f, 1.0f)), 1));
	///colorQuad(quads[7], TextureColor(createAtlasUVRange(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)), 1));
	///colorQuad(quads[8], TextureColor(createAtlasUVRange(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)), 1));
	///uploadUVBuffer();
	//--------------------------------------------------------------------------------------------------------
	//GUI Lines
	//size_t line = gl::GUI::createLine(glm::vec2(-0.495f, 0.075f), glm::vec2(-0.205f, 0.075f), 8);
	
	colorQuad(quitButtonQuad, PlainColor(7));
	colorQuad(playButtonQuad, PlainColor(7));
	size_t quit_button = addButtonQuad(quitButtonQuad);
	//------------WINDOW-------------------------------------------------------------------
	float window_edge_marging = 0.005f;
	float window_width = 0.2f;
	float window_height = 0.2f;
	///size_t windowQuad = createQuad(-0.5f, 0.2f, window_width, window_height);
	///size_t headQuad = createQuad(-0.5f, 0.2f, 0.2f, 0.04f);
	///colorQuad(windowQuad, PlainColor(1));
	///colorQuad(headQuad, PlainColor(6));
	///size_t head_button = addButtonQuad(headQuad);
	///size_t pull_button_press = EventSignal<MouseEvent>(MouseEvent(head_button, MouseKeyEvent(0, KeyCondition(1, 0)))).index();
	///size_t pull_button_release = EventSignal<MouseEvent>(MouseEvent(head_button, MouseKeyEvent(0, KeyCondition(0, 0)))).index();
	///EventSignal<CursorEvent>::reserve(2);
	///size_t pull_button_leave = EventSignal<CursorEvent>(CursorEvent(head_button, 0)).index();
	///size_t pull_button_enter = EventSignal<CursorEvent>(CursorEvent(head_button, 1)).index();
	///
	///size_t moveWindowSignal = EventSignal<MouseEvent>(MouseEvent(head_button, MouseKeyEvent(0, KeyCondition(1, 0))), Signal(1).index()).index();
	///EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(0, 0)), moveWindowSignal, 0);
	///
	///Functor<AnySignalGate, void, glm::vec2&, glm::vec2&> movePullQuadFunc(*moveQuad, getQuad(headQuad).pos, cursorFrameDelta, { moveWindowSignal });
	///
	///Functor<AnySignalGate, void, glm::vec2&, glm::vec2&> moveWindowQuadFunc(*moveQuad, getQuad(windowQuad).pos, cursorFrameDelta, { moveWindowSignal });
//Lights
	gl::Lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.95f, 1.0f, 10.0f));
	gl::Lighting::createLight(glm::vec4(3.0f, 5.0f, 3.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.2f, 10.0f));


	//-----------SLIDER-----------------------------------------------------
	
	float slider_height = 0.05f;
	float slider_width = window_width - window_edge_marging*2.0f;
	float slider_slide_width = 0.01f;
	SliderColors<PlainColor>slidercolors(0, 1);
	SliderQuads sliderquads1(-0.9f, 0.4f, slider_width, slider_height, slider_slide_width);
	SliderQuads sliderquads2(-0.9f, 0.3f, slider_width, slider_height, slider_slide_width);
	SliderQuads sliderquads3(-0.9f, 0.2f, slider_width, slider_height, slider_slide_width);
	SliderQuads sliderquads4(-0.9f, 0.1f, slider_width, slider_height, slider_slide_width);
	Slider<float&, PlainColor, PinSlideType>sliderx(sliderquads1, slidercolors, SliderControl(0.0f, 1.0f, gl::Lighting::getLightColor(1).x));
	Slider<float&, PlainColor, PinSlideType>slidery(sliderquads2, slidercolors, SliderControl(0.0f, 1.0f, gl::Lighting::getLightColor(1).y));
	Slider<float&, PlainColor, PinSlideType>sliderz(sliderquads3, slidercolors, SliderControl(0.0f, 1.0f, gl::Lighting::getLightColor(1).z));
	Slider<float&, PlainColor, BarSlideType>sliderw(sliderquads4, slidercolors, SliderControl(0.0f, 100.0f, gl::Lighting::getLightColor(1).w));
	

	

	//Signals
	reserveKeySignals(11);
	KeySignal key_esc(GLFW_KEY_ESCAPE);
	KeySignal key_c(GLFW_KEY_C);
	KeySignal key_g(GLFW_KEY_G);
	KeySignal key_h(GLFW_KEY_H);
	KeySignal key_i(GLFW_KEY_I);
	KeySignal key_w(GLFW_KEY_W);
	KeySignal key_s(GLFW_KEY_S);
	KeySignal key_a(GLFW_KEY_A);
	KeySignal key_d(GLFW_KEY_D);
	KeySignal key_space(GLFW_KEY_SPACE);
	KeySignal key_z(GLFW_KEY_Z);

	

	EventSignal<MouseKeyEvent>::reserve(4);
	size_t rmb_press = EventSignal<MouseKeyEvent>(MouseKeyEvent(1, KeyCondition(1, 0))).index();
	size_t rmb_release = EventSignal<MouseKeyEvent>(MouseKeyEvent(1, KeyCondition(0, 0))).index();
	size_t lmb_press = EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(1, 0))).index();
	size_t lmb_release = EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(0, 0))).index();

	EventSignal<MouseEvent>::reserve(6);
	size_t quit_button_press = EventSignal<MouseEvent>(MouseEvent(quit_button, MouseKeyEvent(0, KeyCondition(1, 0)))).index();
	
	
	
	
	
	

	//general functions
	Functor<AnySignalGate, void> quitFunc(quit, { key_esc.press, quit_button_press });
	Functor<AnySignalGate, void> hideCursorFunc(App::Input::toggleCursor, { key_c.press, rmb_press, rmb_release });
	Functor<AnySignalGate, void> trackMouseFunc(gl::Camera::toggleLook, { key_c.press, rmb_press, rmb_release });
	Functor<AnySignalGate, void> (gl::Debug::toggleGrid, { key_g.press, key_h.press });
	Functor<AnySignalGate, void> (gl::Debug::toggleCoord, { key_h.press });

	size_t forward_signal = EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_W, KeyCondition(1, 0)), Signal(1).index()).index();
	EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_W, KeyCondition(0, 0)), forward_signal, 0);
	size_t back_signal = EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_S, KeyCondition(1, 0)), Signal(1).index()).index();
	EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_S, KeyCondition(0, 0)), back_signal, 0);
	size_t left_signal = EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_A, KeyCondition(1, 0)), Signal(1).index()).index();
	EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_A, KeyCondition(0, 0)), left_signal, 0);
	size_t right_signal = EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_D, KeyCondition(1, 0)), Signal(1).index()).index();
	EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_D, KeyCondition(0, 0)), right_signal, 0);
	size_t up_signal = EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_SPACE, KeyCondition(1, 0)), Signal(1).index()).index();
	EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_SPACE, KeyCondition(0, 0)), up_signal, 0);
	size_t down_signal = EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_Z, KeyCondition(1, 0)), Signal(1).index()).index();
	EventSignal<KeyEvent>(KeyEvent(GLFW_KEY_Z, KeyCondition(0, 0)), down_signal, 0);

	Functor<AnySignalGate, void> forwardFunc(gl::Camera::forward, { forward_signal });
	Functor<AnySignalGate, void> backFunc(gl::Camera::back, { back_signal });
	Functor<AnySignalGate, void> leftFunc(gl::Camera::left, { left_signal });
	Functor<AnySignalGate, void> rightFunc(gl::Camera::right, { right_signal });
	Functor<AnySignalGate, void> upFunc(gl::Camera::up, { up_signal });
	Functor<AnySignalGate, void> downFunc(gl::Camera::down, { down_signal });


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

		gl::updateGeneralUniformBuffer();
		gl::Models::updateBuffers();
		gl::Lighting::updateLightIndexRangeBuffer();
		gl::Lighting::updateLightDataBuffer();
		gl::GUI::Text::updateCharStorage();
		gl::GUI::updateColorQuads();
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
		gl::GUI::renderColorQuads();
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
	gl::GUI::fetchButtonMap();
	Input::updateCursor();
	Input::getCursorEvents();
	Input::fetchGLFWEvents();
	Input::checkEvents();
	Input::callFunctors();
	Input::resetSignals();
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
