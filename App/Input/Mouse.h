#pragma once
#include "Input.h"
#include "Keys.h"

namespace App {
	namespace Input{
		extern std::vector<KeyEvent> mouseButtonEventBuffer;
		extern std::vector<EventSlot<KeyEvent>> allMouseButtonSignals;
		extern KeyCondition mouseButtons[3];
		extern glm::dvec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		extern int scroll;
		extern int track_mouse;
		extern int centerCursor;
		extern unsigned int mouseEventSlotOffset;
		extern unsigned int mouseEventSlotCount;
		void toggleTrackMouseRef(int* pMode);
		void toggleTrackMouse();
		void checkMouseEvents();
	}
}