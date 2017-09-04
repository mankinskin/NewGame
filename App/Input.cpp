#include "Global\stdafx.h"
#include "Input.h"
#include "Global\App.h"
#include "ContextWindow.h"
#include "Global\Debug.h"
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
std::vector<App::Input::ButtonEvent> App::Input::buttonEventBuffer;
std::vector<App::Input::Signal<App::Input::KeyEvent>> App::Input::allKeySignals;
std::vector<unsigned> App::Input::allSignals;
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
	
	//init Event Signals
	allKeySignals.reserve(16);	//Movement
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_W, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_W, 0, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_S, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_S, 0, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_A, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_A, 0, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_D, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_D, 0, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_SPACE, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_SPACE, 0, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_Z, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_Z, 0, 0)));
	//Misc
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_ESCAPE, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_C, 1, 0)));
	allKeySignals.push_back(Signal<KeyEvent>(TOTAL_SIGNAL_COUNT++, KeyEvent(GLFW_KEY_I, 1, 0)));
	//bind signals to slots
	allSignals.resize(TOTAL_SIGNAL_COUNT);
	App::Input::FuncSlot<void>::bind(0, { 0 });//forward
	App::Input::FuncSlot<void>::bind(1, { 2 });//back
	App::Input::FuncSlot<void>::bind(2, { 4 });//left
	App::Input::FuncSlot<void>::bind(3, { 6 });//right
	App::Input::FuncSlot<void>::bind(4, { 8 });//up
	App::Input::FuncSlot<void>::bind(5, { 10 });//down
	App::Input::FuncSlot<void>::bind(6, { 1, 3 });//stop_z
	App::Input::FuncSlot<void>::bind(7, { 5, 7 });//stop_x
	App::Input::FuncSlot<void>::bind(8, { 9, 11 });//stop_y

	App::Input::FuncSlot<void>::bind(9, { 12 });//exit
	App::Input::FuncSlot<void>::bind(10, { 13 });//setCameraMode
	App::Input::FuncSlot<void>::bind(11, { 14 });//togglePrintInfo
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
	size_t buttonEventCount = buttonEventBuffer.size();
	size_t keyEventCount = keyEventBuffer.size();
	std::vector<void(*)()> cbs;
	cbs.resize(buttonEventCount + keyEventCount);
	
	unsigned int to_call = 0;

	//--------------------------------------------
	//keyEvents
	for (unsigned int e = 0; e < keyEventCount; ++e) {
		KeyEvent& kev = keyEventBuffer[e];
		for (unsigned int ks = 0; ks < allKeySignals.size(); ++ks) {
			Signal<KeyEvent>& sig = allKeySignals[ks];
			if (sig.evnt == kev) {
				allSignals[sig.index] = 1;
			}
		}
		
	}
	keyEventBuffer.clear();
	//-----------------------------------------
	//buttons
	for (unsigned int e = 0; e < buttonEventCount; ++e) {
		ButtonEvent& bev = buttonEventBuffer[e];
		
	}
	buttonEventBuffer.clear();
	//--------------------------------------------

	if (!to_call)
		return;
	callbackBuffer.resize(to_call);
	std::memcpy(&callbackBuffer[0], &cbs[0], sizeof(void(*)())*to_call);
}

void App::Input::frame_end()
{
	cursorFrameDelta = glm::vec2();
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
	FuncSlot<void>::invoke_all();
	//FuncSlot<void, int*>::invoke_all();
	std::fill(allSignals.begin(), allSignals.end(), 0);
}

void App::Input::fetchGLFWEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	
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
	mouseButtons[pButton] = KeyCondition(pAction, pMods);
}

void App::Input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{
	scroll = (int)pY;
}

void App::Input::toggleCenterCursorRef(int* pMode)
{
	if (*pMode) {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	*pMode = !*pMode;
}

void App::Input::toggleCenterCursor()
{
	toggleCenterCursorRef(&centerCursor);
}

