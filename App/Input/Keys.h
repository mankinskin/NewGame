#pragma once

namespace App {
	namespace Input {

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
		inline bool operator!=(KeyCondition const & l, KeyCondition const& r) {
			return l.action != r.action || l.mods != r.mods;
		}

		class KeyEvent {
		public:
			KeyEvent()
				:key(-1), change(KeyCondition()) {}
			KeyEvent(int pKey, KeyCondition pChange)
				:key(pKey), change(pChange) {}
			KeyEvent(int pKey, int pAction, int pMods)
				:key(pKey), change(KeyCondition(pAction, pMods)) {}

			int key;
			KeyCondition change;
		};

		inline bool operator==(KeyEvent const & l, KeyEvent const& r) {
			return l.key == r.key && l.change == r.change;
		}
		struct KeySignal {
			KeySignal(int pKey);

			size_t press;
			size_t release;
		};
		void reserveKeySignals(size_t pCount);

		void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
		void char_Callback(GLFWwindow* window, size_t pCodepoint);
	}
}