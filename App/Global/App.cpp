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
#include <OpenGL/BaseGL/Framebuffer.h>
#include "../Input/Controller.h"
#include <OpenGL\GUI\Group.h>
#include <OpenGL\GUI\Line.h>
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
        //Buttons
        unsigned int quitButtonQuad = gl::GUI::createQuad(-1.0f, -0.9f, 0.2f, 0.1f);
        unsigned int playButtonQuad = gl::GUI::createQuad(-1.0f, -0.7f, 0.2f, 0.1f);
       
	unsigned int sliderButtonQuad = gl::GUI::createQuad(-0.495f, 0.1f, 0.02f, 0.05f);
	unsigned int pullButtonQuad = gl::GUI::createQuad(-0.495f, 0.19f, 0.29f, 0.05f);
        unsigned int windowQuad = gl::GUI::createQuad(-0.5f, 0.2f, 0.3f, 0.3f);
	//Gui Lines
	//unsigned int line = gl::GUI::createLine(glm::vec2(0.4f, 0.3f), glm::vec2(0.5f, 0.3), 8);

        gl::GUI::colorQuad(quitButtonQuad, 1);
        gl::GUI::colorQuad(playButtonQuad, 5);
        gl::GUI::colorQuad(windowQuad, 7);
        gl::GUI::colorQuad(pullButtonQuad, 6);
	gl::GUI::colorQuad(sliderButtonQuad, 6);

	unsigned int window_group = gl::GUI::createGroup(windowQuad);
	gl::GUI::addGroupQuad(window_group, windowQuad);
	gl::GUI::addGroupQuad(window_group, pullButtonQuad);
	gl::GUI::addGroupQuad(window_group, sliderButtonQuad);
	//gl::GUI::addGroupLine(window_group, line);
        unsigned quit_button = gl::GUI::addButton(quitButtonQuad);
        unsigned pull_button = gl::GUI::addButton(pullButtonQuad);
	unsigned slider_button = gl::GUI::addButton(sliderButtonQuad);
        //Signals
        using Input::EventSlot;
        using Input::KeyEvent;
        using Input::KeyCondition;
        using Input::MouseKeyEvent;
        using Input::MouseEvent;
        using Input::FuncSlot;
        using Input::Signal;
        EventSlot<KeyEvent>::reserve_slots(28);//reserve EventSlots for EventType KeyEvent
        Input::ClickInterface<KeyEvent> esc_slot(KeyEvent(GLFW_KEY_ESCAPE, KeyCondition(1, 0)));
        EventSlot<KeyEvent> c_press_slot(KeyEvent(GLFW_KEY_C, KeyCondition(1, 0)));
        EventSlot<KeyEvent> g_press_slot(KeyEvent(GLFW_KEY_G, KeyCondition(1, 0)));
        EventSlot<KeyEvent> h_press_slot(KeyEvent(GLFW_KEY_H, KeyCondition(1, 0)));
        EventSlot<KeyEvent> i_press_slot(KeyEvent(GLFW_KEY_I, KeyCondition(1, 0)));

        //camera controller
	Input::ToggleInterface<KeyEvent> w_slot(KeyEvent(GLFW_KEY_W, KeyCondition(1, 0)), KeyEvent(GLFW_KEY_W, KeyCondition(0, 0)));

        EventSlot<KeyEvent> s_press_slot  (KeyEvent(GLFW_KEY_S, KeyCondition(1, 0)));
        EventSlot<KeyEvent> s_release_slot(KeyEvent(GLFW_KEY_S, KeyCondition(0, 0)));

        EventSlot<KeyEvent> a_press_slot  (KeyEvent(GLFW_KEY_A, KeyCondition(1, 0)));
        EventSlot<KeyEvent> a_release_slot(KeyEvent(GLFW_KEY_A, KeyCondition(0, 0)));

        EventSlot<KeyEvent> d_press_slot  (KeyEvent(GLFW_KEY_D, KeyCondition(1, 0)));
        EventSlot<KeyEvent> d_release_slot(KeyEvent(GLFW_KEY_D, KeyCondition(0, 0)));

        EventSlot<KeyEvent> space_press_slot  (KeyEvent(GLFW_KEY_SPACE, KeyCondition(1, 0)));
        EventSlot<KeyEvent> space_release_slot(KeyEvent(GLFW_KEY_SPACE, KeyCondition(0, 0)));

        EventSlot<KeyEvent> z_press_slot  (KeyEvent(GLFW_KEY_Z, KeyCondition(1, 0)));
        EventSlot<KeyEvent> z_release_slot(KeyEvent(GLFW_KEY_Z, KeyCondition(0, 0)));

        FuncSlot<void> moveForwardFunc(gl::Camera::forward);
        FuncSlot<void> moveBackFunc(gl::Camera::back);
        FuncSlot<void> stopZFunc(gl::Camera::stop_z);

        FuncSlot<void> moveLeftFunc(gl::Camera::left);
        FuncSlot<void> moveRightFunc(gl::Camera::right);
        FuncSlot<void> stopXFunc(gl::Camera::stop_x);

        FuncSlot<void> moveUpFunc(gl::Camera::up);
        FuncSlot<void> moveDownFunc(gl::Camera::down);
	FuncSlot<void> stopYFunc(gl::Camera::stop_y);

        moveForwardFunc.listen({ w_slot.on_event.signal_index });
        moveBackFunc.listen({ s_press_slot.signal_index });
        stopZFunc.listen({ w_slot.off_event.signal_index , s_release_slot.signal_index });

        moveLeftFunc.listen({ a_press_slot.signal_index });
        moveRightFunc.listen({ d_press_slot.signal_index });
        stopXFunc.listen({ a_release_slot.signal_index , d_release_slot.signal_index });

        moveUpFunc.listen({ space_press_slot.signal_index });
        moveDownFunc.listen({ z_press_slot.signal_index });
        stopYFunc.listen({ space_release_slot.signal_index , z_release_slot.signal_index });
        //
        EventSlot<KeyEvent> up_press_slot(KeyEvent(GLFW_KEY_UP, KeyCondition(1, 0)));
        EventSlot<KeyEvent> up_release_slot(KeyEvent(GLFW_KEY_UP, KeyCondition(0, 0)));
        EventSlot<KeyEvent> down_press_slot(KeyEvent(GLFW_KEY_DOWN, KeyCondition(1, 0)));
        EventSlot<KeyEvent> down_release_slot(KeyEvent(GLFW_KEY_DOWN, KeyCondition(0, 0)));
        EventSlot<KeyEvent> left_press_slot(KeyEvent(GLFW_KEY_LEFT, KeyCondition(1, 0)));
        EventSlot<KeyEvent> left_release_slot(KeyEvent(GLFW_KEY_LEFT, KeyCondition(0, 0)));
        EventSlot<KeyEvent> right_press_slot(KeyEvent(GLFW_KEY_RIGHT, KeyCondition(1, 0)));
        EventSlot<KeyEvent> right_release_slot(KeyEvent(GLFW_KEY_RIGHT, KeyCondition(0, 0)));
        EventSlot<KeyEvent> o_press_slot(KeyEvent(GLFW_KEY_O, KeyCondition(1, 0)));
        EventSlot<KeyEvent> o_release_slot(KeyEvent(GLFW_KEY_O, KeyCondition(0, 0)));
        EventSlot<KeyEvent> l_press_slot(KeyEvent(GLFW_KEY_L, KeyCondition(1, 0)));
        EventSlot<KeyEvent> l_release_slot(KeyEvent(GLFW_KEY_L, KeyCondition(0, 0)));


        EventSlot<MouseKeyEvent>::reserve_slots(4);
        EventSlot<MouseKeyEvent> lmb_press_slot(MouseKeyEvent(GLFW_MOUSE_BUTTON_1, KeyCondition(1, 0)));
        EventSlot<MouseKeyEvent> lmb_release_slot(MouseKeyEvent(GLFW_MOUSE_BUTTON_1, KeyCondition(0, 0)));

        EventSlot<MouseKeyEvent> rmb_press_slot(MouseKeyEvent(GLFW_MOUSE_BUTTON_2, KeyCondition(1, 0)));
        EventSlot<MouseKeyEvent> rmb_release_slot(MouseKeyEvent(GLFW_MOUSE_BUTTON_2, KeyCondition(0, 0)));

        EventSlot<MouseEvent>::reserve_slots(3);
	EventSlot<MouseEvent> quit_button_press(MouseEvent(quit_button, MouseKeyEvent(0, 1, 0)));
        EventSlot<MouseEvent> pull_button_press(MouseEvent(pull_button, MouseKeyEvent(0, 1, 0)));
	EventSlot<MouseEvent> slider_button_press(MouseEvent(slider_button, MouseKeyEvent(0, 1, 0)));

        FuncSlot<void, unsigned int>::reserve_slots(5);
	FuncSlot<void, unsigned int> sliderFollowFunc(gl::GUI::moveQuadByMouseDelta, sliderButtonQuad);
	FuncSlot<void, unsigned int> sliderstartFollowFunc(Input::startRule, slider_button_press.signal_index);
	FuncSlot<void, unsigned int> sliderstopFollowFunc(Input::stopRule, slider_button_press.signal_index);

	FuncSlot<void, unsigned int> quadFollowFunc(gl::GUI::moveQuadGroupByMouseDelta, window_group);
        FuncSlot<void, unsigned int> quadstartFollowFunc(Input::startRule, pull_button_press.signal_index);
        FuncSlot<void, unsigned int> quadstopFollowFunc(Input::stopRule, pull_button_press.signal_index);

	sliderFollowFunc.listen({ slider_button_press.signal_index });
	sliderstartFollowFunc.listen({ slider_button_press.signal_index });
	sliderstopFollowFunc.listen({ lmb_release_slot.signal_index });

	quadFollowFunc.listen({ pull_button_press.signal_index });
	quadstartFollowFunc.listen({ pull_button_press.signal_index  });
        quadstopFollowFunc.listen({ lmb_release_slot.signal_index });

        FuncSlot<void>::reserve_slots(21);

        FuncSlot<void> quitFunc(quit);//define functions
        FuncSlot<void> toggleMouseTrack(Input::toggleTrackMouse);
        FuncSlot<void> toggleGrid(gl::Debug::toggleGrid);
        FuncSlot<void> toggleCoord(gl::Debug::toggleCoord);
        FuncSlot<void> togglePrintInfo(App::Debug::togglePrintInfo);

        
        //
        FuncSlot<void> moveLightForwardFunc([]() {App::light_mov.z = -1.0f; });
        FuncSlot<void> moveLightBackFunc([]() {App::light_mov.z = 1.0f; });
        FuncSlot<void> stopLightZFunc([]() {App::light_mov.z = 0.0f; });

        FuncSlot<void> moveLightLeftFunc([]() {App::light_mov.x = -1.0f; });
        FuncSlot<void> moveLightRightFunc([]() {App::light_mov.x = 1.0f; });
        FuncSlot<void> stopLightXFunc([]() {App::light_mov.x = 0.0f; });

        FuncSlot<void> moveLightUpFunc([]() {App::light_mov.y = 1.0f; });
        FuncSlot<void> moveLightDownFunc([]() {App::light_mov.y = -1.0f; });
        FuncSlot<void> stopLightYFunc([]() {App::light_mov.y = 0.0f; });
        
        quitFunc.listen({ esc_slot.click_event.signal_index, quit_button_press.signal_index });
        toggleMouseTrack.listen({ c_press_slot.signal_index, rmb_press_slot.signal_index, rmb_release_slot.signal_index });
        toggleGrid.listen({g_press_slot.signal_index});
        toggleCoord.listen({ h_press_slot.signal_index });
        togglePrintInfo.listen({ i_press_slot.signal_index });
        
        
        //
        moveLightForwardFunc.listen({ up_press_slot.signal_index });
        moveLightBackFunc.listen({ down_press_slot.signal_index });
        stopLightZFunc.listen({ up_release_slot.signal_index , down_release_slot.signal_index });

        moveLightLeftFunc.listen({ left_press_slot.signal_index });
        moveLightRightFunc.listen({ right_press_slot.signal_index });
        stopLightXFunc.listen({ left_release_slot.signal_index , right_release_slot.signal_index });

        moveLightUpFunc.listen({ o_press_slot.signal_index });
        moveLightDownFunc.listen({ l_press_slot.signal_index });
        stopLightYFunc.listen({ o_release_slot.signal_index , l_release_slot.signal_index });

        Input::set_up_lock(w_slot.on_event.signal_index, w_slot.off_event.signal_index, { s_press_slot.signal_index, s_release_slot.signal_index });
        Input::set_up_lock(s_press_slot.signal_index, s_release_slot.signal_index, { w_slot.on_event.signal_index, w_slot.off_event.signal_index });

        Input::set_up_lock(a_press_slot.signal_index, a_release_slot.signal_index, { d_press_slot.signal_index, d_release_slot.signal_index });
        Input::set_up_lock(d_press_slot.signal_index, d_release_slot.signal_index, { a_press_slot.signal_index, a_release_slot.signal_index });

        Input::set_up_lock(space_press_slot.signal_index, space_release_slot.signal_index, { z_press_slot.signal_index, z_release_slot.signal_index });
        Input::set_up_lock(z_press_slot.signal_index, z_release_slot.signal_index, { space_press_slot.signal_index, space_release_slot.signal_index });
        //
        Input::set_up_lock(up_press_slot.signal_index, up_release_slot.signal_index, { down_press_slot.signal_index, down_release_slot.signal_index });
        Input::set_up_lock(down_press_slot.signal_index, down_release_slot.signal_index, { up_press_slot.signal_index, up_release_slot.signal_index });

        Input::set_up_lock(left_press_slot.signal_index, left_release_slot.signal_index, { right_press_slot.signal_index, right_release_slot.signal_index });
        Input::set_up_lock(right_press_slot.signal_index, right_release_slot.signal_index, { left_press_slot.signal_index, left_release_slot.signal_index });

        Input::set_up_lock(o_press_slot.signal_index, o_release_slot.signal_index, { l_press_slot.signal_index, z_release_slot.signal_index });
        Input::set_up_lock(l_press_slot.signal_index, l_release_slot.signal_index, { o_press_slot.signal_index, o_release_slot.signal_index });
        
        
        //Text
        gl::GUI::Text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
        unsigned int pl_tb = gl::GUI::Text::createTextbox(playButtonQuad, 0, 0);
        unsigned int qu_tb = gl::GUI::Text::createTextbox(quitButtonQuad, 0, 0);
        
        gl::GUI::Text::setTextboxString(qu_tb, gl::GUI::Text::String("QUIT"));
        gl::GUI::Text::setTextboxString(pl_tb, gl::GUI::Text::String("Play"));
        gl::GUI::Text::loadTextboxes();

        //Lights
        gl::Lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.95f, 0.1f, 10.0f));
        gl::Lighting::createLight(glm::vec4(3.0f, 5.0f, 3.0f, 1.0f), glm::vec4(0.5f, 0.0f, 1.0f, 10.0f));

        
        while (state == App::MainMenu) {

                light_pos += light_mov;
                gl::Lighting::setLightPos(1, light_pos);
                gl::Camera::look(Input::cursorFrameDelta);
		gl::GUI::updateGroups();
                gl::Camera::update();
                gl::updateGeneralUniformBuffer();
                gl::Models::updateBuffers();
                gl::Lighting::updateLightIndexRangeBuffer();
                gl::Lighting::updateLightDataBuffer();
                gl::GUI::Text::updateCharStorage();
                gl::GUI::updateColoredQuads();
                gl::GUI::updateQuadBuffer();
		gl::GUI::updateLines();

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
		limitFPS();

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
