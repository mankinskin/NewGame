#include "..\Global\stdafx.h"
#include "Input.h"
#include "..\Global\App.h"
#include "..\ContextWindow.h"
#include "..\Global\Debug.h"
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <functional>


glm::dvec2 App::Input::relativeCursorPosition;
glm::uvec2 App::Input::absoluteCursorPosition;

App::Input::KeyCondition App::Input::mouseButtons[3];
int App::Input::scroll;
glm::vec2 App::Input::cursorFrameDelta;
int App::Input::centerCursor;
std::vector<App::Input::ButtonCondition> App::Input::allButtonStates;
std::vector<glm::vec4> App::Input::allDetectors;
std::vector<void(*)()> App::Input::callbackBuffer;
std::vector<gl::GUI::Quad> App::Input::quadLoadBuffer;
std::vector<App::Input::KeyEvent> App::Input::keyEventBuffer;
std::vector<App::Input::KeyEvent> App::Input::mouseButtonEventBuffer;
std::vector<App::Input::ButtonEvent> App::Input::buttonEventBuffer;
unsigned int App::Input::mouseSignalOffset;
unsigned int App::Input::mouseSignalCount;
unsigned int App::Input::keySignalOffset;
unsigned int App::Input::keySignalCount;
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
	FuncSlot<void> setCameraModeSlot(toggleCenterCursor);
	FuncSlot<void> togglePrintInfoSlot(App::Debug::togglePrintInfo);
	FuncSlot<void> toggleCoordinateSysSlot(gl::Debug::toggleCoord);
	FuncSlot<void> toggleGridSlot(gl::Debug::toggleGrid);
	//init Event Signals
	//-----------------------
	//Mouse Buttons
	mouseSignalOffset = 0;
	mouseSignalCount = 2;
	setCameraModeSlot.listen({ TOTAL_SIGNAL_COUNT , TOTAL_SIGNAL_COUNT+1});//setCameraMode
	EventSlot<KeyEvent>::create(KeyEvent(GLFW_MOUSE_BUTTON_2, 1, 0));
	EventSlot<KeyEvent>::create(KeyEvent(GLFW_MOUSE_BUTTON_2, 0, 0));
	//-----------------------------------------
	//Keys
	//Movement
	keySignalOffset = mouseSignalOffset + mouseSignalCount;
	keySignalCount = 16;
	
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


	moveForwardSlot.listen({ w_press });//forward
	stopZSlot.listen({ w_release, s_release });//stop_z
	moveBackwardSlot.listen({ s_press});//back
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
	stopYSlot.listen({ space_release, z_release});//stop_y
	signal_lock(space_press, { z_press, z_release });
	signal_unlock(space_release, { z_press, z_release });
	signal_lock(z_press, { space_press, space_release });
	signal_unlock(z_release, { space_press, space_release });

	//Misc
	exitProgramSlot.listen({ EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_ESCAPE, 1, 0)) });//exit
	togglePrintInfoSlot.listen({ EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_I, 1, 0)) });//togglePrintInfo
	toggleCoordinateSysSlot.listen({ EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_H, 1, 0)) });//toggleCoordSys
	toggleGridSlot.listen({ EventSlot<KeyEvent>::create(KeyEvent(GLFW_KEY_G, 1, 0)) });//toggleGrid
	
	//--------------------------------
	//Buttons

	//--------------------------------
	//bind signals to slots
	allSignals.reserve(TOTAL_SIGNAL_COUNT);//for ordered access
	allSignalSlots.resize(TOTAL_SIGNAL_COUNT);
	allSignalLocks.resize(TOTAL_SIGNAL_COUNT);
}



void App::Input::calculateDetectionRanges()
{
	using gl::GUI::allSizes;
	using gl::GUI::allPositions;
	using gl::GUI::allQuads;
	size_t quadCount = quadLoadBuffer.size();
	allDetectors.resize(quadCount);
	allButtonStates.resize(quadCount);
	for (unsigned int q = 0; q < quadCount; ++q) {
		allDetectors[q] = glm::vec4(
			allPositions[allQuads[quadLoadBuffer[q]].pos].x, 
			allPositions[allQuads[quadLoadBuffer[q]].pos].y, 
			allPositions[allQuads[quadLoadBuffer[q]].pos].x + allSizes[allQuads[quadLoadBuffer[q]].size].x,
			allPositions[allQuads[quadLoadBuffer[q]].pos].y - allSizes[allQuads[quadLoadBuffer[q]].size].y);
	}
}

void App::Input::clearDetectionRanges()
{
	allDetectors.clear();
}

void App::Input::checkEvents() {
	//TODO find a way to manage ranges of different element types
	size_t keyEventCount = keyEventBuffer.size();
	size_t mouseKeyEventCount = mouseButtonEventBuffer.size();
	size_t keyEventSignalCount = EventSlot<KeyEvent>::instance_count();
	
	static std::vector<unsigned int> signalBuffer;
	std::vector<unsigned int> signals;
	signals.resize(keyEventCount + mouseKeyEventCount);
	unsigned int signalCount = 0;
	
	for (unsigned int e = 0; e < keyEventCount; ++e) {
		KeyEvent& kev = keyEventBuffer[e];
		for (unsigned int ks = 0; ks < keySignalCount; ++ks) {
			EventSlot<KeyEvent>& slot = EventSlot<KeyEvent>::get(keySignalOffset + ks);
			if (slot.evnt == kev) {
				signals[signalCount++] = slot.index;
			}
		}
	}
	keyEventBuffer.clear();


	for (unsigned int e = 0; e < mouseKeyEventCount; ++e) {
		KeyEvent& kev = mouseButtonEventBuffer[e];
		for (unsigned int ks = 0; ks < mouseSignalCount; ++ks) {
			EventSlot<KeyEvent>& slot = EventSlot<KeyEvent>::get(ks);
			if (slot.evnt == kev) {
				signals[signalCount++] = slot.index;
			}
		}

	}
	mouseButtonEventBuffer.clear();
	signals.resize(signalCount);

	signalBuffer.insert(signalBuffer.end(), signals.begin(), signals.end());
	signalCount = 0;
	signals.resize(signalBuffer.size());
	for (unsigned int& sig : signalBuffer) {
		if (allSignalLocks[sig]) {
			signals[signalCount++] = sig;
		}
		else {
			allSignals.push_back(sig);
			allSignalSlots[sig] = 1;
		}
	}
	signals.resize(signalCount);
	signalBuffer = signals;
}

void App::Input::end()
{
	scroll = 0;
	if (!centerCursor) {
		cursorFrameDelta = glm::vec2();
	}
	if (glfwWindowShouldClose(App::mainWindow.window)) {
		App::state = App::State::Exit;
	}
}

void App::Input::callFunctions()
{
	for (unsigned int& sig : allSignals) {
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
	for (unsigned int s = 0; s < allSignals.size(); ++s) {
		unsigned int& sig = allSignals[s];
		auto& to_lock = signalLockBindings[sig];
		for (unsigned int l = 0; l < to_lock.second.size() * allSignalSlots[sig]; ++l) {
			allSignalLocks[to_lock.second[l]] = to_lock.first;
		}
	}
	allSignals.clear();
}

void App::Input::fetchGLFWEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	cursorFrameDelta = glm::vec2();
	glfwPollEvents();
}

void App::Input::fetchButtonEvents()
{
	size_t button_count = allDetectors.size();
	std::vector<ButtonEvent> evnts(button_count);
	unsigned int evnt_count = 0;
	for (size_t b = 0; b < button_count; ++b) {
		//compare all previous button states to all new ones
		ButtonCondition& prevState = allButtonStates[b];
		int inside = is_inside_quad((glm::vec2)relativeCursorPosition, allDetectors[b]);
		ButtonCondition& newState =
			ButtonCondition(mouseButtons[0].action * inside, inside);
		if (prevState != newState) {
			ButtonCondition change;
			change.action = prevState.action == newState.action ? 0 : newState.action;
			change.in = prevState.in == newState.in ? 0 : newState.in;
			evnts[evnt_count++] = ButtonEvent(b, change);
		}
		prevState = newState;
	}
	if (!evnt_count)
		return;
	buttonEventBuffer.resize(evnt_count);
	std::memcpy(&buttonEventBuffer[0], &evnts[0], sizeof(ButtonEvent)*evnt_count);
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

void App::Input::toggleCenterCursorRef(int* pMode)
{
	*pMode = !*pMode;
	if (*pMode) {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	
}

void App::Input::toggleCenterCursor()
{
	toggleCenterCursorRef(&centerCursor);
}

