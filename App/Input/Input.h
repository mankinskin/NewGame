#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>
#include "Signal.h"
#include "Keys.h"
namespace App {
    namespace Input {
	using Key = int;


	class KeyCondition {
	public:
	    KeyCondition()
		:action(0), // 0 = release, 1 = press
		mods(0) {}
	    KeyCondition(int pAction, int pMods)
		:action(pAction),
		mods(pMods)
	    {}

	    int action;
	    int mods;
	};
	inline bool operator==(KeyCondition const & l, KeyCondition const& r) {
	    return l.action == r.action && l.mods == r.mods;
	}

	class KeyEvent {
	public:
	    KeyEvent()
		:key(-1), change(KeyCondition()) {}
	    KeyEvent(Key pKey, KeyCondition pChange)
		:key(pKey), change(pChange) {}
	    KeyEvent(Key pKey, int pAction, int pMods)
		:key(pKey), change(KeyCondition(pAction, pMods)) {}

	    Key key;
	    KeyCondition change;
	};
	inline bool operator==(KeyEvent const & l, KeyEvent const& r) {
	    return l.key == r.key && l.change == r.change;
	}

	extern std::vector<KeyEvent> keyEventBuffer;

	struct MouseKeyEvent {
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
	    CursorEvent(size_t pButton, size_t pAction)
		:button_index(pButton), action(pAction) {}
	    size_t button_index;//0 if none
	    size_t action;
	};
	inline bool operator==(CursorEvent const & l, CursorEvent const& r) {
	    return l.button_index == r.button_index && l.action == r.action;
	}
	extern std::vector<MouseKeyEvent> mouseKeyEventBuffer;
	void init();
	void fetchGLFWEvents();
	void checkEvents();
	void end();

	static void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
	static void char_Callback(GLFWwindow* window, size_t pCodepoint);
	static void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
	static void cursorEnter_Callback(GLFWwindow * window, int pEntered);
	static void mouseKey_Callback(GLFWwindow* window, int pKey, int pAction, int pMods);
	static void scroll_Callback(GLFWwindow* window, double pX, double pY);
    }
}
