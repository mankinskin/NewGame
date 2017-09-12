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
std::unordered_map<unsigned int, std::vector<unsigned int>> App::Input::signalLockBindings;
std::unordered_map<unsigned int, std::vector<unsigned int>> App::Input::signalUnlockBindings;
std::vector<int> App::Input::allSignalLocks;
std::vector<unsigned int> App::Input::allSignalSlots;
unsigned App::Input::TOTAL_SIGNAL_COUNT;

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
	unsigned int rmb_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_MOUSE_BUTTON_2, 1, 0));
	unsigned int rmb_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_MOUSE_BUTTON_2, 0, 0));
	unsigned int esc_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_ESCAPE, 1, 0));
	unsigned int i_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_I, 1, 0));
	keyEventSlotCount = 4;
	unsigned int play_button_press = EventSlot<ButtonEvent>::create(ButtonEvent(0, 1, 0));
	unsigned int quit_button_press = EventSlot<ButtonEvent>::create(ButtonEvent(1, 1, 0));
	
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

	FuncSlot<void> exitProgramSlot(App::quit);
	FuncSlot<void> setCameraModeSlot(toggleTrackMouse);
	FuncSlot<void> togglePrintInfoSlot(App::Debug::togglePrintInfo);
	FuncSlot<void> toggleCoordinateSysSlot(gl::Debug::toggleCoord);
	FuncSlot<void> toggleGridSlot(gl::Debug::toggleGrid);
	
	
	//init Event Signals
	//-----------------------
	//Mouse Buttons
	mouseEventSlotOffset = EventSlot<KeyEvent>::instance_count();
	unsigned int rmb_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_MOUSE_BUTTON_2, 1, 0));
	unsigned int rmb_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_MOUSE_BUTTON_2, 0, 0));
	mouseEventSlotCount = EventSlot<KeyEvent>::instance_count() - mouseEventSlotOffset;
	//-----------------------------------------
	//Keys
	//Movement
	keyEventSlotOffset = mouseEventSlotOffset + mouseEventSlotCount;
	unsigned int c_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_C, 1, 0));
	unsigned int c_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_C, 0, 0));
	unsigned int w_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_W, 1, 0));
	unsigned int w_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_W, 0, 0));
	unsigned int s_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_S, 1, 0));
	unsigned int s_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_S, 0, 0));
	unsigned int a_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_A, 1, 0));
	unsigned int a_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_A, 0, 0));
	unsigned int d_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_D, 1, 0));
	unsigned int d_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_D, 0, 0));
	unsigned int space_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_SPACE, 1, 0));
	unsigned int space_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_SPACE, 0, 0));
	unsigned int z_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_Z, 1, 0));
	unsigned int z_release = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_Z, 0, 0));
	unsigned int esc_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_ESCAPE, 1, 0));
	unsigned int i_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_I, 1, 0));
	unsigned int h_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_H, 1, 0));
	unsigned int g_press = EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_G, 1, 0));
	
	keyEventSlotCount = EventSlot<KeyEvent>::instance_count() - keyEventSlotOffset;
	
	setCameraModeSlot.listen({ rmb_press , rmb_release, c_press });//setCameraMode
	moveForwardSlot.listen({ w_press });//forward
	stopZSlot.listen({ w_release, s_release });//stop_z
	moveBackwardSlot.listen({ s_press });//back
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


	//--------------------------------
	//Buttons
	unsigned int quit_button_press = EventSlot<ButtonEvent>::create(ButtonEvent(0, 1, 0));

	//Misc
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
	unsigned int passed_signals = 0;
	for (unsigned int& sig : rejectedSignals) {
		if (!allSignalLocks[sig]) {
			allSignalSlots[sig] = 1;
			signalBuffer[passed_signals++] = sig;
		}
		else {
			rejected.push_back(sig);
		}
	}
	signalBuffer.resize(passed_signals);
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
	std::fill(allSignalSlots.begin(), allSignalSlots.end(), 0);
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



