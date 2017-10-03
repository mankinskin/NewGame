#include "..\Global\stdafx.h"
#include "Input.h"
#include "..\Global\App.h"
#include "..\Global\Debug.h"
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <functional>
#include "Keys.h"
#include "Mouse.h"
#include <OpenGL\GUI\Buttons.h>
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
	//glfwSetCursorPosCallback(App::mainWindow.window, cursorPosition_Callback);
	glfwSetCursorEnterCallback(App::mainWindow.window, cursorEnter_Callback);
	glfwSetMouseButtonCallback(App::mainWindow.window, mouseKey_Callback);
	glfwSetScrollCallback(App::mainWindow.window, scroll_Callback);
}



void App::Input::clearSignals() {
	EventSlot<KeyEvent>::clear();
	FuncSlot<void>::clear();
	allSignalSlots.clear();
	allSignalLocks.clear();
	signalLockBindings.clear();
	signalUnlockBindings.clear();
}



void App::Input::fetchGLFWEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	
	glfwPollEvents();
        //update mouse
        double pX = 0.0f;
        double pY = 0.0f;
        glfwGetCursorPos(App::mainWindow.window, &pX, &pY);
        absoluteCursorPosition = glm::uvec2(std::clamp((unsigned int)pX, (unsigned int)0, mainWindow.width - 1), std::clamp(mainWindow.height - (unsigned int)pY - 1, (unsigned int)0, mainWindow.height - 1));
        glm::vec2 newRelativeCursorPosition = glm::vec2(((pX / (float)App::mainWindow.width)*2.0f) - 1.0f, 1.0f - (pY / (float)App::mainWindow.height)*2.0f);
        cursorFrameDelta = glm::vec2(  (newRelativeCursorPosition.x - relativeCursorPosition.x),
                                        (newRelativeCursorPosition.y - relativeCursorPosition.y) );
        relativeCursorPosition = newRelativeCursorPosition;
}

void App::Input::checkEvents() {
	
	checkKeyEvents();
	checkMouseEvents();
        

	//set signals if they are not locked
	rejectedSignals.insert(rejectedSignals.end(), signalBuffer.begin(), signalBuffer.end());
	std::vector<unsigned int> rejected;
	rejected.reserve(rejectedSignals.size());
	signalBuffer.resize(rejectedSignals.size());
	unsigned int passed = 0;
	for (unsigned int& sig : rejectedSignals) {
 		if (!allSignalLocks[sig]) {
			allSignalSlots[sig].on = 1;
			signalBuffer[passed++] = sig;
		}
		else {
			rejected.push_back(sig);
		}
	}
	signalBuffer.clear();
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
	for (FuncSlot<void>& inst : FuncSlot<void>::instances) {
		for (unsigned s : inst.signal_bindings) {
			if (allSignalSlots[s].on) {
				inst.invoke();
				break;
			}
		}
	}

        for (FuncSlot<void, unsigned int>& inst : FuncSlot<void, unsigned int>::instances) {
                for (unsigned s : inst.signal_bindings) {
                        if (allSignalSlots[s].on) {
                                inst.invoke();
                                break;
                        }
                }
        }
		//any other function template here
	
	//reset signals and lock signals
        for(auto& to_lock : signalLockBindings){
                if (allSignalSlots[to_lock.first].on) {
                        for (unsigned int l = 0; l < to_lock.second.size(); ++l) {
                                allSignalLocks[to_lock.second[l]] = 1;
                        }
                }
	}
        for (auto& to_unlock : signalUnlockBindings) {
                if (allSignalSlots[to_unlock.first].on) {
                        for (unsigned int l = 0; l < to_unlock.second.size(); ++l) {
                                allSignalLocks[to_unlock.second[l]] = 0;
                        }
                }
	}
	
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
        
}

void App::Input::cursorEnter_Callback(GLFWwindow* window, int pEntered)
{
	//Entered = 1 if entered, on exit = 0
}

void App::Input::mouseKey_Callback(GLFWwindow * window, int pKey, int pAction, int pMods)
{
	mouseKeyEventBuffer.push_back(MouseKeyEvent(pKey, pAction, pMods));//fetch mouse key events
}

void App::Input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{
	scroll = (int)pY;
}



