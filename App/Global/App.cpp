#include "stdafx.h"
#include "App.h"
#include "Debug.h"
#include "..\Input\Input.h"
#include "..\Input\Event.h"
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
        //Buttons
        unsigned int quitButtonQuad = gl::GUI::createQuad(-1.0f, -0.8f, 0.2f, 0.1f);
        gl::GUI::colorQuad(quitButtonQuad, 0);
        unsigned quit_button = Input::addButton(quitButtonQuad);
        //Signals
        using Input::EventSlot;
        using Input::KeyEvent;
        using Input::KeyCondition;
        using Input::ButtonEvent;
        using Input::ButtonCondition;
        using Input::MouseKeyEvent;
        using Input::FuncSlot;

        EventSlot<KeyEvent>::reserve_slots(15);//reserve EventSlots for EventType KeyEvent
        EventSlot<KeyEvent> esc_press_slot(KeyEvent(GLFW_KEY_ESCAPE, KeyCondition(1, 0)));
        EventSlot<KeyEvent> c_press_slot(KeyEvent(GLFW_KEY_C, KeyCondition(1, 0)));
        EventSlot<KeyEvent> g_press_slot(KeyEvent(GLFW_KEY_C, KeyCondition(1, 0)));

        EventSlot<KeyEvent> w_press_slot  (KeyEvent(GLFW_KEY_W, KeyCondition(1, 0)));
        EventSlot<KeyEvent> w_release_slot(KeyEvent(GLFW_KEY_W, KeyCondition(0, 0)));
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

        EventSlot<MouseKeyEvent>::reserve_slots(2);
        EventSlot<MouseKeyEvent> rmb_press_slot(MouseKeyEvent(GLFW_MOUSE_BUTTON_2, KeyCondition(1, 0)));
        EventSlot<MouseKeyEvent> rmb_release_slot(MouseKeyEvent(GLFW_MOUSE_BUTTON_2, KeyCondition(0, 0)));

        EventSlot<ButtonEvent>::reserve_slots(1);
        EventSlot<ButtonEvent> quit_button_press_slot(ButtonEvent(quit_button, ButtonCondition(1, 0)));

        FuncSlot<void>::reserve_slots(2);
        FuncSlot<void> quitFunc(quit);//define functions
        FuncSlot<void> toggleMouseTrack(Input::toggleTrackMouse);

        FuncSlot<void> moveForwardFunc(gl::Camera::forward);
        FuncSlot<void> moveBackFunc(gl::Camera::back);
        FuncSlot<void> stopZFunc(gl::Camera::stop_z);

        FuncSlot<void> moveLeftFunc(gl::Camera::left);
        FuncSlot<void> moveRightFunc(gl::Camera::right);
        FuncSlot<void> stopXFunc(gl::Camera::stop_x);

        FuncSlot<void> moveUpFunc(gl::Camera::up);
        FuncSlot<void> moveDownFunc(gl::Camera::down);
        FuncSlot<void> stopYFunc(gl::Camera::stop_y);

        quitFunc.listen({ esc_press_slot.signal_index, quit_button_press_slot.signal_index });
        toggleMouseTrack.listen({ c_press_slot.signal_index, rmb_press_slot.signal_index, rmb_release_slot.signal_index });
        
        moveForwardFunc.listen({ w_press_slot.signal_index });
        moveBackFunc.listen({ s_press_slot.signal_index });
        stopZFunc.listen({ w_release_slot.signal_index , s_release_slot.signal_index });
        
        moveLeftFunc.listen({ a_press_slot.signal_index });
        moveRightFunc.listen({ d_press_slot.signal_index });
        stopXFunc.listen({ a_release_slot.signal_index , d_release_slot.signal_index });
        
        moveUpFunc.listen({ space_press_slot.signal_index });
        moveDownFunc.listen({ z_press_slot.signal_index });
        stopYFunc.listen({ space_release_slot.signal_index , z_release_slot.signal_index });


        Input::set_up_lock(w_press_slot.signal_index, w_release_slot.signal_index, { s_press_slot.signal_index, s_release_slot.signal_index });
        Input::set_up_lock(s_press_slot.signal_index, s_release_slot.signal_index, { w_press_slot.signal_index, w_release_slot.signal_index });

        Input::set_up_lock(a_press_slot.signal_index, a_release_slot.signal_index, { d_press_slot.signal_index, d_release_slot.signal_index });
        Input::set_up_lock(d_press_slot.signal_index, d_release_slot.signal_index, { a_press_slot.signal_index, a_release_slot.signal_index });

        Input::set_up_lock(space_press_slot.signal_index, space_release_slot.signal_index, { z_press_slot.signal_index, z_release_slot.signal_index });
        Input::set_up_lock(z_press_slot.signal_index, z_release_slot.signal_index, { space_press_slot.signal_index, space_release_slot.signal_index });
        //Text
        gl::GUI::Text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
        unsigned int tb = gl::GUI::Text::createTextbox(quitButtonQuad, 0, 0);
        gl::GUI::Text::setTextboxString(tb, gl::GUI::Text::String("QUIT"));
        gl::GUI::Text::loadTextboxes();

        //Lights
        gl::Lighting::reservePointLightSpace(2);
        gl::Lighting::createLight(glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 100.0));
        gl::Lighting::createLight(glm::vec4(1.0, 1.0, -1.0, 1.0), glm::vec4(0.5, 0.8, 1.0, 100.0));
        
        while (state == App::MainMenu) {

                fetchInput();
                gl::Camera::look(Input::cursorFrameDelta);
                gl::Camera::update();
                gl::updateGeneralUniformBuffer();
                gl::GUI::updateQuadBuffer();
                gl::GUI::updateColoredQuads();
                gl::Lighting::updateLightDataBuffer();
                gl::Lighting::updateLightIndexRangeBuffer();
                gl::GUI::Text::updateCharStorage();
                gl::Models::updateBuffers();
                
                gl::Debug::drawGrid();
                gl::Models::render();
                gl::Lighting::renderLights();
                
                
                gl::renderFrame();
                gl::GUI::renderColoredQuads();
                gl::GUI::Text::renderGlyphs();
                glfwSwapBuffers(mainWindow.window);
                

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


		gl::renderFrame();
		
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
