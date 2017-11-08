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
#include <OpenGL\GUI\UI\Widget.h>
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
	size_t playButtonQuad = createQuad(-1.0f, -0.7f, 0.2f, 0.07f);
	size_t quitButtonQuad = createQuad(-1.0f, -0.9f, 0.2f, 0.07f);
	

	using WindowPreset = WidgetPreset<Element<9>, Element<1>>;
	using Window = Widget<Element<9>, Element<1>>;
	using SingleQuadColor = ColorPreset<Element<1>, ConstColor>;
	using BorderCornerQuadColor = ColorPreset<Element<9>, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor>;
	using WindowColors = ColorPreset<Window, BorderCornerQuadColor, SingleQuadColor>;

	ElementPreset<Element<9>> boco(0.0f, 0.0f, 1.0f, 1.0f, 0.005f);
	ElementPreset<Element<1>> sing(0.005f, -0.005f*(1600.0f/850.0f), 0.99f, 0.05f);
	
	Window win(boco, sing); 
	Window win1(boco, sing);

	WindowColors winColors(
		BorderCornerQuadColor(ConstColor(1), ConstColor(1), ConstColor(1), ConstColor(1), ConstColor(7), ConstColor(1), ConstColor(1), ConstColor(1), ConstColor(1)), 
		SingleQuadColor(ConstColor(1)));
	color(win, winColors);
	color(win1, winColors);


	size_t window_header_button = addButton(win.get<1>().quads[0]);
	size_t window1_header_button = addButton(win1.get<1>().quads[0]);

	size_t header_button_press = create_button_signal<MouseEvent, MouseKeyEvent>
		(MouseEvent(window_header_button, MouseKeyEvent(0, KeyCondition(1, 0))),
			MouseKeyEvent(0, KeyCondition(0, 0)));

	Functor<AnySignalGate, Window, glm::vec2&>(moveWidget<Window>, win, cursorFrameDelta, { header_button_press });
	
	size_t header1_button_press = create_button_signal<MouseEvent, MouseKeyEvent>
		(MouseEvent(window1_header_button, MouseKeyEvent(0, KeyCondition(1, 0))),
			MouseKeyEvent(0, KeyCondition(0, 0)));

	Functor<AnySignalGate, Window, glm::vec2&>(moveWidget<Window>, win1, cursorFrameDelta, { header1_button_press });

	//Signals
	colorQuad(playButtonQuad, ConstColor(7));
	colorQuad(quitButtonQuad, ConstColor(7));
	

	gl::Lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.95f, 1.0f, 10.0f));
	gl::Lighting::createLight(glm::vec4(3.0f, 5.0f, 3.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.2f, 10.0f));
	
	size_t quit_button = addButton(quitButtonQuad);
	size_t play_button = addButton(playButtonQuad);
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
	size_t play_button_press = EventSignal<MouseEvent>(MouseEvent(play_button, MouseKeyEvent(0, KeyCondition(1, 0)))).index();
	size_t quit_button_press = EventSignal<MouseEvent>(MouseEvent(quit_button, MouseKeyEvent(0, KeyCondition(1, 0)))).index();
	

	//general functions
	Functor<AnySignalGate>(quit, { key_esc.press, quit_button_press });
	Functor<AnySignalGate>(App::Input::toggleCursor, { play_button_press, key_c.press, rmb_press, rmb_release });
	Functor<AnySignalGate>(gl::Camera::toggleLook, { play_button_press, key_c.press, rmb_press, rmb_release });
	Functor<AnySignalGate>(gl::Debug::toggleGrid, { key_g.press });
	Functor<AnySignalGate>(gl::Debug::toggleCoord, { key_h.press });
	Functor<AnySignalGate>(App::Debug::togglePrintInfo, { key_i.press });
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

	Functor<AnySignalGate> forwardFunc(gl::Camera::forward, { forward_signal });
	Functor<AnySignalGate> backFunc(gl::Camera::back, { back_signal });
	Functor<AnySignalGate> leftFunc(gl::Camera::left, { left_signal });
	Functor<AnySignalGate> rightFunc(gl::Camera::right, { right_signal });
	Functor<AnySignalGate> upFunc(gl::Camera::up, { up_signal });
	Functor<AnySignalGate> downFunc(gl::Camera::down, { down_signal });


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
		glViewport(0, 0, size_t(gl::screenWidth*gl::resolution), size_t(gl::screenHeight*gl::resolution));
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
		fetchInput();
		glfwSwapBuffers(mainWindow.window);

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
	Input::fetchGLFWEvents();
	Input::getCursorEvents();
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
