#pragma once
#include "Keys.h"

namespace App {
	namespace Input{
                struct MouseKeyEvent{
                        MouseKeyEvent()
                                :key(-1), change(KeyCondition()) {}
                        MouseKeyEvent(Key pKey, KeyCondition pChange)
                                :key(pKey), change(pChange) {}
                        MouseKeyEvent(Key pKey, int pAction, int pMods)
                                :key(pKey), change(KeyCondition(pAction, pMods)) {}
                        Key key;
                        KeyCondition change;
                };
                inline bool operator==(MouseKeyEvent const & l, MouseKeyEvent const& r) {
                        return l.key == r.key && l.change == r.change;
                }
		extern std::vector<MouseKeyEvent> mouseButtonEventBuffer;
		extern KeyCondition mouseButtons[3];
		extern glm::dvec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		extern int scroll;
		extern int track_mouse;
		extern int centerCursor;
		void toggleTrackMouseRef(int* pMode);
		void toggleTrackMouse();
		void checkMouseEvents();
	}
}