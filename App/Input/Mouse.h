#pragma once
#include <glm/glm.hpp>
#include "Keys.h"
namespace App {
	namespace Input {
		
		struct MouseKeyEvent {
			MouseKeyEvent()
				:key(-1), change(KeyCondition()) {}
			MouseKeyEvent(int pKey, KeyCondition pChange)
				:key(pKey), change(pChange) {}
			MouseKeyEvent(int pKey, int pAction, int pMods)
				:key(pKey), change(KeyCondition(pAction, pMods)) {}
			int key;
			KeyCondition change;
		};
		inline bool operator==(MouseKeyEvent const & l, MouseKeyEvent const& r) {
			return l.key == r.key && l.change == r.change;
		}
		struct MouseEvent {
			MouseEvent()
				:button_index(-1), key_evnt(MouseKeyEvent()) {}
			MouseEvent(size_t pButton, MouseKeyEvent pEvnt)
				:button_index(pButton), key_evnt(pEvnt) {}
			size_t button_index;//0 if none
			MouseKeyEvent key_evnt;
		};
		inline bool operator==(MouseEvent const & l, MouseEvent const& r) {
			return l.button_index == r.button_index && l.key_evnt == r.key_evnt;
		}
		struct CursorEvent {
			CursorEvent()
				:button_index(-1), action(0) {}
			CursorEvent(size_t pButton, int pAction)
				:button_index(pButton), action(pAction) {}
			size_t button_index;//0 if none
			int action;
		};
		inline bool operator==(CursorEvent const & l, CursorEvent const& r) {
			return l.button_index == r.button_index && l.action == r.action;
		}

		extern glm::vec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		void updateCursor();
		void resetMouse();
		void toggleCursor();
		void getCursorEvents();

		void mouseKey_Callback(GLFWwindow* window, int pKey, int pAction, int pMods);
		void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
		void cursorEnter_Callback(GLFWwindow * window, int pEntered);
		void scroll_Callback(GLFWwindow* window, double pX, double pY);
	}
}