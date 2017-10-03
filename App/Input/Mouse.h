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
		
                
                struct MouseEvent {
                        MouseEvent()
                                :button_index(-1), key_evnt(MouseKeyEvent()) {}
                        MouseEvent(unsigned int pButton, MouseKeyEvent pEvnt)
                                :button_index(pButton), key_evnt(pEvnt) {}
                        unsigned int button_index;//0 if none
                        MouseKeyEvent key_evnt;
                };
                inline bool operator==(MouseEvent const & l, MouseEvent const& r) {
                        return l.button_index == r.button_index && l.key_evnt == r.key_evnt;
                }
                
                extern std::vector<MouseKeyEvent> mouseKeyEventBuffer;
                extern std::vector<MouseEvent> mouseEventBuffer;
		extern KeyCondition mouseKeys[3];
		extern glm::vec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		extern int scroll;
		extern int track_mouse;
		extern int centerCursor;
                extern unsigned int hovered_button;//button under cursor (0 if none)
		void toggleTrackMouseRef(int* pMode);
		void toggleTrackMouse();
		void checkMouseEvents();
	}
}