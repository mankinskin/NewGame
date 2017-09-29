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
#include <algorithm>

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

void App::Input::init()
{
        puts("Initilizing GLFW Input...\n");
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
	allSignalSlots.clear();
	allSignalLocks.clear();
	signalLockBindings.clear();
	signalUnlockBindings.clear();
}



void App::Input::fetchEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	cursorFrameDelta = glm::vec2();
	glfwPollEvents();
        fetchButtonEvents();
}

void App::Input::checkEvents() {
	
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
        absoluteCursorPosition = glm::uvec2((unsigned int)pX, mainWindow.height - (unsigned int)pY);
        
	glm::dvec2 newRelativeCursorPosition = glm::dvec2((pX/(double)App::mainWindow.width)*2.0 -1.0, -1.0*(((pY / (double)App::mainWindow.height)*2.0 - 1.0)));

	cursorFrameDelta = glm::vec2(newRelativeCursorPosition - relativeCursorPosition);
	
	relativeCursorPosition = newRelativeCursorPosition;
        
        printf("aX: %i\naY: %i\n---\nrX: %f\nrY: %f\n\n", absoluteCursorPosition.x, absoluteCursorPosition.y, relativeCursorPosition.x, relativeCursorPosition.y);
}

void App::Input::cursorEnter_Callback(GLFWwindow* window, int pEntered)
{
	//Entered = 1 if entered, on exit = 0
}

void App::Input::mouseButton_Callback(GLFWwindow * window, int pButton, int pAction, int pMods)
{
	mouseButtonEventBuffer.push_back(MouseKeyEvent(pButton, pAction, pMods));
	mouseButtons[pButton] = KeyCondition(pAction, pMods);
}

void App::Input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{
	scroll = (int)pY;
}



