#include "../Global/stdafx.h"
#include "Mouse.h"
#include "..\Global\App.h"
#include "..\ContextWindow.h"
#include <OpenGL\GUI\Buttons.h>
#include <algorithm>
#include "Signal.h"
glm::vec2 App::Input::relativeCursorPosition;
glm::uvec2 App::Input::absoluteCursorPosition;
std::vector<App::Input::MouseKeyEvent> App::Input::mouseKeyEventBuffer;
std::vector<App::Input::MouseEvent> App::Input::mouseEventBuffer;
App::Input::KeyCondition App::Input::mouseKeys[3];
int App::Input::scroll;
glm::vec2 App::Input::cursorFrameDelta;
int App::Input::centerCursor;
int App::Input::track_mouse;
unsigned int App::Input::hovered_button = 0;

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
	using namespace SignalInternal;
	unsigned int cursor_pos = App::mainWindow.width * (App::Input::absoluteCursorPosition.y) + (App::Input::absoluteCursorPosition.x);
        unsigned int now_hovered_button = gl::GUI::buttonIndexMap[cursor_pos];

	for (MouseKeyEvent& kev : mouseKeyEventBuffer){
		for (unsigned int ks = 0; ks < EventSlot<MouseKeyEvent>::instance_count(); ++ks) {
			EventSlot<MouseKeyEvent>& slot = EventSlot<MouseKeyEvent>::get_instance(ks);
			if (slot.evnt == kev) {
				allSignals[slot.signalIndex].signaled = true;
			}
		}
	}

	for (MouseKeyEvent& kev : mouseKeyEventBuffer) {
                for (unsigned int ks = 0; ks < EventSlot<MouseEvent>::instance_count(); ++ks) {
                        EventSlot<MouseEvent>& slot = EventSlot<MouseEvent>::get_instance(ks);
                        if (slot.evnt == MouseEvent(hovered_button, kev)) {
				allSignals[slot.signalIndex].signaled = true;
                        }
                        if (hovered_button!= now_hovered_button && slot.evnt == MouseEvent(now_hovered_button, kev)) {
				allSignals[slot.signalIndex].signaled = true;
                        }
                }
        }

        hovered_button = now_hovered_button;
        mouseKeyEventBuffer.clear();
}
