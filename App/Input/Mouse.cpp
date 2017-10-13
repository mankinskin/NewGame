#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Mouse.h"
#include "..\Global\App.h"
#include "..\ContextWindow.h"
#include <OpenGL\GUI\UI\Buttons.h>
#include <algorithm>
#include "Signal.h"
#include "Input.h"
glm::vec2 App::Input::relativeCursorPosition;
glm::uvec2 App::Input::absoluteCursorPosition;
glm::vec2 App::Input::cursorFrameDelta;

std::vector<App::Input::MouseKeyEvent> App::Input::mouseKeyEventBuffer;
App::Input::KeyCondition mouseKeys[3];
int scroll = 0;
int disableCursor = 0;
size_t hovered_button = 0;

void App::Input::updateCursor() {
	//update cursor pos
	double ax = 0.0;//absolute positions
	double ay = 0.0;
	
	//GLFW window coordinates are from top to bottom all others are bottom to top
	glfwGetCursorPos(App::mainWindow.window, &ax, &ay);
	float rx = (float)ax;
	float ry = (float)ay;
	ax = std::clamp(ax, 0.0, (double)mainWindow.width - 1.0);
	ay = std::clamp(ay, 0.0, (double)mainWindow.height - 1.0);
	if (!disableCursor) {
		rx = ax;//clamp relative positions too if cursor is not disabled
		ry = ay;
	}
	absoluteCursorPosition = glm::uvec2((unsigned int)ax, mainWindow.height - (unsigned int)ay - 1);

	rx = ((rx / (float)App::mainWindow.width)*2.0f) - 1.0f;
	ry = 1.0f - (ry / (float)App::mainWindow.height)*2.0f;
	glm::vec2 newRelativeCursorPosition = glm::vec2(rx, ry);
	cursorFrameDelta = glm::vec2((newRelativeCursorPosition.x - relativeCursorPosition.x),
		(newRelativeCursorPosition.y - relativeCursorPosition.y));
	relativeCursorPosition = newRelativeCursorPosition;

	//printf("%f\n%f\n\n\n", pX, pY);
	//printf("%i\n%i\n\n\n", absoluteCursorPosition.x, absoluteCursorPosition.y);
}

void App::Input::checkMouseEvents()
{
	using namespace SignalInternal;
	size_t now_hovered_button = gl::GUI::readButtonIndexMap(absoluteCursorPosition.x, absoluteCursorPosition.y);

	for (MouseKeyEvent& kev : mouseKeyEventBuffer) {
		for (size_t ks = 0; ks < EventSlot<MouseKeyEvent>::instance_count(); ++ks) {
			EventSlot<MouseKeyEvent>& slot = EventSlot<MouseKeyEvent>::get_instance(ks);
			if (slot.evnt == kev) {
				allSignals[slot.signalIndex].signaled = true;
			}
		}
	}

	for (MouseKeyEvent& kev : mouseKeyEventBuffer) {
		for (size_t ks = 0; ks < EventSlot<MouseEvent>::instance_count(); ++ks) {
			EventSlot<MouseEvent>& slot = EventSlot<MouseEvent>::get_instance(ks);

			if (slot.evnt == MouseEvent(hovered_button, kev)) {
				allSignals[slot.signalIndex].signaled = true;
			}
			if (hovered_button != now_hovered_button) {
				if (slot.evnt == MouseEvent(now_hovered_button, kev)) {
					allSignals[slot.signalIndex].signaled = true;
				}
			}
		}
	}
	for (size_t ks = 0; ks < EventSlot<CursorEvent>::instance_count(); ++ks) {
		EventSlot<CursorEvent>& slot = EventSlot<CursorEvent>::get_instance(ks);
		if (hovered_button != now_hovered_button) {
			//button change
			if (slot.evnt.button_index == hovered_button && slot.evnt.action == 0) {
				allSignals[slot.signalIndex].signaled = true;
			}
			else if (slot.evnt.button_index == now_hovered_button && slot.evnt.action == 1) {
				allSignals[slot.signalIndex].signaled = true;
			}
		}
	}

	hovered_button = now_hovered_button;
	updateCursor();
	mouseKeyEventBuffer.clear();
}


void App::Input::resetMouse()
{
	scroll = 0;
}

void App::Input::toggleCursor()
{
	disableCursor = !disableCursor;
	if (disableCursor) {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


}