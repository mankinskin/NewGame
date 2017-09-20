#include "..\Global\stdafx.h"
#include "Input.h"
#include "..\Global\App.h"
#include "..\Global\Debug.h"
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <functional>
#include "Keys.h"
#include "Mouse.h"
#include "Buttons.h"


std::vector<void(*)()> App::Input::callbackBuffer;
std::vector<unsigned int> App::Input::signalBuffer;
std::vector<unsigned int> App::Input::rejectedSignals;

/*
button-pipeline
to set up
- bind quads as buttons
- calculateQuadRanges

every frame
- fetch Events 
	- iterate through buttons
	- get button state and compare it to previous state
	- if different -> generate event
- check for relevant events/wheater callbacks should be called
	- iterate through all events
	- check buttonID-specific range in functor array
- callButtons
*/

void App::Input::initMenuSignals() {
	Input::clearSignals();
	App::Input::FuncSlot<void>::reserve_slots(12);
	FuncSlot<void> startProgramSlot(App::run);
	FuncSlot<void> exitProgramSlot(App::quit);
	FuncSlot<void> togglePrintInfoSlot(App::Debug::togglePrintInfo);
	mouseEventSlotOffset = 0;
	mouseEventSlotCount = 0;
	keyEventSlotOffset = 0;
	unsigned int rmb_press = 0;
	unsigned int rmb_release = 0; 
	unsigned int esc_press = 0;
	unsigned int i_press = 0;
	
	EventSlot<KeyEvent>(rmb_press, KeyEvent(GLFW_MOUSE_BUTTON_2, 1, 0));
	
	EventSlot<KeyEvent>(rmb_release, KeyEvent(GLFW_MOUSE_BUTTON_2, 0, 0));
	EventSlot<KeyEvent>(esc_press, KeyEvent(GLFW_KEY_ESCAPE, 1, 0));
	 EventSlot<KeyEvent>(i_press, KeyEvent(GLFW_KEY_I, 1, 0));
	keyEventSlotCount = 4;
	unsigned int play_button_press = 0;
	unsigned int quit_button_press = 0;
	EventSlot<ButtonEvent>(play_button_press, ButtonEvent(0, 1, 0));
	EventSlot<ButtonEvent>(quit_button_press, ButtonEvent(1, 1, 0));
	
	startProgramSlot.listen({ play_button_press });
	exitProgramSlot.listen({ esc_press, quit_button_press });//exit
	togglePrintInfoSlot.listen({ i_press });//togglePrintInfo

	allSignalSlots.resize(TOTAL_SIGNAL_COUNT);
	allSignalLocks.resize(TOTAL_SIGNAL_COUNT);
}

void App::Input::initGameGUISignals() {
	Input::clearSignals();
	//Init function slots
	App::Input::FuncSlot<void>::reserve_slots(12);

	FuncSlot<void> moveForwardSlot(gl::Camera::forward);
	FuncSlot<void> moveBackwardSlot(gl::Camera::back);
	FuncSlot<void> moveLeftSlot(gl::Camera::left);
	FuncSlot<void> moveRightSlot(gl::Camera::right);
	FuncSlot<void> moveUpSlot(gl::Camera::up);
	FuncSlot<void> moveDownSlot(gl::Camera::down);
	FuncSlot<void> stopZSlot(gl::Camera::stop_z);
	FuncSlot<void> stopXSlot(gl::Camera::stop_x);
	FuncSlot<void> stopYSlot(gl::Camera::stop_y);

	FuncSlot<void> moveLightForwardSlot([]() {App::light_mov += glm::vec3(0.0f, 0.0f, -1.0f); });
	FuncSlot<void> moveLightBackwardSlot([]() {App::light_mov += glm::vec3(0.0f, 0.0f, 1.0f); });
	FuncSlot<void> moveLightLeftSlot([]() {App::light_mov += glm::vec3(-1.0f, 0.0f, 0.0f); });
	FuncSlot<void> moveLightRightSlot([]() {App::light_mov += glm::vec3(1.0f, 0.0f, 0.0f); });
	FuncSlot<void> moveLightUpSlot([]() {App::light_mov += glm::vec3(0.0f, 1.0f, 0.0f); });
	FuncSlot<void> moveLightDownSlot([]() {App::light_mov += glm::vec3(0.0f, -1.0f, 0.0f); });
	FuncSlot<void> stopLightZSlot([]() {App::light_mov.z = 0.0f; });
	FuncSlot<void> stopLightXSlot([]() {App::light_mov.x = 0.0f; });
	FuncSlot<void> stopLightYSlot([]() {App::light_mov.y = 0.0f; });

	FuncSlot<void> exitProgramSlot(App::quit);
	FuncSlot<void> menuProgramSlot(App::mainmenu);
	FuncSlot<void> setCameraModeSlot(toggleTrackMouse);
	FuncSlot<void> togglePrintInfoSlot(App::Debug::togglePrintInfo);
	FuncSlot<void> toggleCoordinateSysSlot(gl::Debug::toggleCoord);
	FuncSlot<void> toggleGridSlot(gl::Debug::toggleGrid);
	
	
	//init Event Signals
	//-----------------------
	//Mouse Buttons
	mouseEventSlotOffset = EventSlot<KeyEvent>::instance_count();
	unsigned int rmb_press = 0;
	unsigned int rmb_release = 0;
	EventSlot<KeyEvent>(rmb_press, KeyEvent(GLFW_MOUSE_BUTTON_2, 1, 0));
	EventSlot<KeyEvent>(rmb_release, KeyEvent(GLFW_MOUSE_BUTTON_2, 0, 0));
	mouseEventSlotCount = EventSlot<KeyEvent>::instance_count() - mouseEventSlotOffset;
	//-----------------------------------------
	//Keys
	//Movement
	keyEventSlotOffset = mouseEventSlotOffset + mouseEventSlotCount;
	unsigned int c_press = 0;       EventSlot<KeyEvent>(c_press, KeyEvent(GLFW_KEY_C, 1, 0));
	unsigned int c_release = 0;     EventSlot<KeyEvent>(c_release, KeyEvent(GLFW_KEY_C, 0, 0));

	unsigned int w_press = 0;       EventSlot<KeyEvent>(w_press, KeyEvent(GLFW_KEY_W, 1, 0));
	unsigned int w_release = 0;     EventSlot<KeyEvent>(w_release, KeyEvent(GLFW_KEY_W, 0, 0));
	unsigned int s_press = 0;       EventSlot<KeyEvent>(s_press, KeyEvent(GLFW_KEY_S, 1, 0));
	unsigned int s_release = 0;     EventSlot<KeyEvent>(s_release, KeyEvent(GLFW_KEY_S, 0, 0));
	unsigned int a_press = 0;       EventSlot<KeyEvent>(a_press, KeyEvent(GLFW_KEY_A, 1, 0));
	unsigned int a_release = 0;     EventSlot<KeyEvent>(a_release, KeyEvent(GLFW_KEY_A, 0, 0));
	unsigned int d_press = 0;       EventSlot<KeyEvent>(d_press, KeyEvent(GLFW_KEY_D, 1, 0));
	unsigned int d_release = 0;     EventSlot<KeyEvent>(d_release, KeyEvent(GLFW_KEY_D, 0, 0));
	unsigned int space_press = 0;   EventSlot<KeyEvent>(space_press, KeyEvent(GLFW_KEY_SPACE, 1, 0));
	unsigned int space_release = 0; EventSlot<KeyEvent>(space_release, KeyEvent(GLFW_KEY_SPACE, 0, 0));
	unsigned int z_press = 0;       EventSlot<KeyEvent>(z_press, KeyEvent(GLFW_KEY_Z, 1, 0));
	unsigned int z_release = 0;     EventSlot<KeyEvent>(z_release, KeyEvent(GLFW_KEY_Z, 0, 0));


	unsigned int up_press = 0;       EventSlot<KeyEvent>(up_press, KeyEvent(GLFW_KEY_UP, 1, 0));
	unsigned int up_release = 0;     EventSlot<KeyEvent>(up_release, KeyEvent(GLFW_KEY_UP, 0, 0));
	unsigned int down_press = 0;       EventSlot<KeyEvent>(down_press, KeyEvent(GLFW_KEY_DOWN, 1, 0));
	unsigned int down_release = 0;     EventSlot<KeyEvent>(down_release, KeyEvent(GLFW_KEY_DOWN, 0, 0));
	unsigned int left_press = 0;       EventSlot<KeyEvent>(left_press, KeyEvent(GLFW_KEY_LEFT, 1, 0));
	unsigned int left_release = 0;     EventSlot<KeyEvent>(left_release, KeyEvent(GLFW_KEY_LEFT, 0, 0));
	unsigned int right_press = 0;       EventSlot<KeyEvent>(right_press, KeyEvent(GLFW_KEY_RIGHT, 1, 0));
	unsigned int right_release = 0;     EventSlot<KeyEvent>(right_release, KeyEvent(GLFW_KEY_RIGHT, 0, 0));
	unsigned int o_press = 0;   EventSlot<KeyEvent>(o_press, KeyEvent(GLFW_KEY_O, 1, 0));
	unsigned int o_release = 0; EventSlot<KeyEvent>(o_release, KeyEvent(GLFW_KEY_O, 0, 0));
	unsigned int l_press = 0;       EventSlot<KeyEvent>(l_press, KeyEvent(GLFW_KEY_L, 1, 0));
	unsigned int l_release = 0;     EventSlot<KeyEvent>(l_release, KeyEvent(GLFW_KEY_L, 0, 0));
	
	unsigned int g_press = 0;       EventSlot<KeyEvent>(g_press, KeyEvent(GLFW_KEY_G, 1, 0));
	unsigned int esc_press = 0;     EventSlot<KeyEvent>(esc_press, KeyEvent(GLFW_KEY_ESCAPE, 1, 0));
	unsigned int i_press = 0;       EventSlot<KeyEvent>(i_press, KeyEvent(GLFW_KEY_I, 1, 0));
	unsigned int h_press = 0;       EventSlot<KeyEvent>(h_press, KeyEvent(GLFW_KEY_H, 1, 0));
	
	keyEventSlotCount = EventSlot<KeyEvent>::instance_count() - keyEventSlotOffset;
	
	setCameraModeSlot.listen({ rmb_press , rmb_release, c_press });//setCameraMode
	
	moveForwardSlot.listen({ w_press });//forward
	moveBackwardSlot.listen({ s_press });//back
	stopZSlot.listen({ w_release, s_release });//stop_z
	signal_lock(w_press, { s_press, s_release });
	signal_unlock(w_release, { s_press, s_release });
	signal_lock(s_press, { w_press, w_release });
	signal_unlock(s_release, { w_press, w_release });

	moveLeftSlot.listen({ a_press });//left
	moveRightSlot.listen({ d_press });//right
	stopXSlot.listen({ a_release, d_release });//stop_x
	signal_lock(a_press, { d_press, d_release });
	signal_unlock(a_release, { d_press, d_release });
	signal_lock(d_press, { a_press, a_release });
	signal_unlock(d_release, { a_press, a_release });

	moveUpSlot.listen({ space_press });//up
	moveDownSlot.listen({ z_press });//down
	stopYSlot.listen({ space_release, z_release });//stop_y
	signal_lock(space_press, { z_press, z_release });
	signal_unlock(space_release, { z_press, z_release });
	signal_lock(z_press, { space_press, space_release });
	signal_unlock(z_release, { space_press, space_release });

	moveLightForwardSlot.listen({ up_press });//forward
	moveLightBackwardSlot.listen({ down_press });//back
	stopLightZSlot.listen({ up_release, down_release });//stop_z
	signal_lock(up_press, { down_press, down_release });
	signal_unlock(up_release, { down_press, down_release });
	signal_lock(down_press, { up_press, up_release });
	signal_unlock(down_release, { up_press, up_release });

	moveLightLeftSlot.listen({ left_press });//left
	moveLightRightSlot.listen({ right_press });//right
	stopLightXSlot.listen({ left_release, right_release });//stop_x
	signal_lock(left_press, { right_press, right_release });
	signal_unlock(left_release, { right_press, right_release });
	signal_lock(right_press, { left_press, left_release });
	signal_unlock(right_release, { left_press, left_release });

	moveLightUpSlot.listen({ o_press });//up
	moveLightDownSlot.listen({ l_press });//down
	stopLightYSlot.listen({ o_release, l_release });//stop_y
	signal_lock(o_press, { l_press, l_release });
	signal_unlock(o_release, { l_press, l_release });
	signal_lock(l_press, { o_press, o_release });
	signal_unlock(l_release, { o_press, o_release });


	//--------------------------------
	//Buttons
	unsigned int quit_button_press = 0; EventSlot<ButtonEvent>(quit_button_press, ButtonEvent(0, 1, 0));
	unsigned int menu_button_press = 0; EventSlot<ButtonEvent>(menu_button_press, ButtonEvent(1, 1, 0));
	//Misc
	menuProgramSlot.listen({ menu_button_press });
	exitProgramSlot.listen({ esc_press, quit_button_press });//exit
	togglePrintInfoSlot.listen({ i_press });//togglePrintInfo
	toggleCoordinateSysSlot.listen({ h_press });//toggleCoordSys
	toggleGridSlot.listen({ g_press });//toggleGrid


	allSignalSlots.resize(TOTAL_SIGNAL_COUNT);
	allSignalLocks.resize(TOTAL_SIGNAL_COUNT);
}

void App::Input::init()
{

	//initializes GLFW input and defines the keys to track
	glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetKeyCallback(App::mainWindow.window, key_Callback);
	glfwSetCharCallback(App::mainWindow.window, char_Callback);
	glfwSetCursorPosCallback(App::mainWindow.window, cursorPosition_Callback);
	glfwSetCursorEnterCallback(App::mainWindow.window, cursorEnter_Callback);
	glfwSetMouseButtonCallback(App::mainWindow.window, mouseButton_Callback);
	glfwSetScrollCallback(App::mainWindow.window, scroll_Callback);
	
	
}



void App::Input::clearSignals() {
	EventSlot<KeyEvent>::clear();
	EventSlot<ButtonEvent>::clear();
	FuncSlot<void>::clear();
	TOTAL_SIGNAL_COUNT = 0;
	allSignalSlots.clear();
	allSignalLocks.clear();
	signalLockBindings.clear();
	signalUnlockBindings.clear();
}



void App::Input::checkEvents() {
	//TODO find a way to manage ranges of different element types
	
	checkKeyEvents();
	checkMouseEvents();
	checkButtonEvents();

	//set signals if they are not locked
	rejectedSignals.insert(rejectedSignals.end(), signalBuffer.begin(), signalBuffer.end());
	std::vector<unsigned int> rejected;
	rejected.reserve(rejectedSignals.size());
	signalBuffer.resize(rejectedSignals.size());
	unsigned int passed = 0;
	for (unsigned int& sig : rejectedSignals) {
 		if (!allSignalLocks[sig]) {
			allSignalSlots[sig].set(1);
			signalBuffer[passed++] = sig;
		}
		else {
			rejected.push_back(sig);
		}
	}
	signalBuffer.resize(passed);
	rejectedSignals = rejected;
}



void App::Input::end()
{
	scroll = 0;
	if (!track_mouse) {
		cursorFrameDelta = glm::vec2();
	}
	if (glfwWindowShouldClose(App::mainWindow.window)) {
		App::state = App::State::Exit;
	}
}

void App::Input::callFunctions()
{
	for (unsigned int& sig : signalBuffer) {
		for (FuncSlot<void>& inst : FuncSlot<void>::instances) {
			for (unsigned s : inst.signal_bindings) {
				if (s == sig) {
					inst.invoke();
					break;
				}
			}
		}
		//any other function template here
	}
	
	//reset signals and lock signals
	for (unsigned int& sig : signalBuffer) {
		auto& to_lock = signalLockBindings[sig];
		for (unsigned int l = 0; l < to_lock.size(); ++l) {
			allSignalLocks[to_lock[l]] = 1;
		}
	}
	for (unsigned int& sig : signalBuffer) {
		auto& to_unlock = signalUnlockBindings[sig];
			for (unsigned int l = 0; l < to_unlock.size(); ++l) {
				allSignalLocks[to_unlock[l]] = 0;
			}
	}
	signalBuffer.clear();
	//reset signals
	//some signals will be set off, others (rules) will stay on untill they are explicitly turned off
	for (unsigned int s = 0; s < allSignalSlots.size(); ++s) {
		allSignalSlots[s].reset();
	}
}

void App::Input::fetchGLFWEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	cursorFrameDelta = glm::vec2();
	glfwPollEvents();
}


void App::Input::key_Callback(GLFWwindow * window, int pKey, int pScancode, int pAction, int pMods)
{
	keyEventBuffer.push_back(KeyEvent(pKey, pAction, pMods));
}

void App::Input::char_Callback(GLFWwindow * window, unsigned int pCodepoint)
{
	//printf("char callBack! Char: %c \n", pCodepoint);
}

void App::Input::cursorPosition_Callback(GLFWwindow * window, double pX, double pY)
{
	
	absoluteCursorPosition = glm::uvec2((unsigned int)pX, (unsigned int)pY);
	glm::dvec2 newRelativeCursorPosition = glm::dvec2((pX/(double)App::mainWindow.width)*2.0 -1.0, -1.0*(((pY / (double)App::mainWindow.height)*2.0 - 1.0)));

	cursorFrameDelta = glm::vec2(newRelativeCursorPosition - relativeCursorPosition);
	
	relativeCursorPosition = newRelativeCursorPosition;
}

void App::Input::cursorEnter_Callback(GLFWwindow* window, int pEntered)
{
	//Entered = 1 if entered, on exit = 0
}

void App::Input::mouseButton_Callback(GLFWwindow * window, int pButton, int pAction, int pMods)
{
	mouseButtonEventBuffer.push_back(KeyEvent(pButton, pAction, pMods));
	mouseButtons[pButton] = KeyCondition(pAction, pMods);
}

void App::Input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{
	scroll = (int)pY;
}



