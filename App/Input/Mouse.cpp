#include "../Global/stdafx.h"
#include "Mouse.h"
#include "..\Global\App.h"
#include "..\ContextWindow.h"


glm::dvec2 App::Input::relativeCursorPosition;
glm::uvec2 App::Input::absoluteCursorPosition;

App::Input::KeyCondition App::Input::mouseButtons[3];
int App::Input::scroll;
glm::vec2 App::Input::cursorFrameDelta;
int App::Input::centerCursor;
int App::Input::track_mouse;
unsigned int App::Input::mouseEventSlotOffset = 0;
unsigned int App::Input::mouseEventSlotCount = 0;

void App::Input::toggleTrackMouseRef(int* pMode)
{
	*pMode = !*pMode;
	if (*pMode) {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void App::Input::toggleTrackMouse()
{
	toggleTrackMouseRef(&track_mouse);
}

void App::Input::checkMouseEvents()
{
	size_t mouseKeyEventCount = mouseButtonEventBuffer.size();
	unsigned int signalOffset = signalBuffer.size();
	signalBuffer.resize(signalOffset + mouseKeyEventCount);
	unsigned int signalCount = 0;

	for (unsigned int e = 0; e < mouseKeyEventCount; ++e) {
		KeyEvent& kev = mouseButtonEventBuffer[e];
		for (unsigned int ks = 0; ks < mouseEventSlotCount; ++ks) {
			EventSlot<KeyEvent>& slot = EventSlot<KeyEvent>::get(mouseEventSlotOffset + ks);
			if (slot.evnt == kev) {
				signalBuffer[signalOffset + signalCount++] = slot.index;
			}
		}

	}
	mouseButtonEventBuffer.clear();
	signalBuffer.resize(signalOffset + signalCount);
}
