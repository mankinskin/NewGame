#pragma once
#include "Input.h"

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
		
		void checkKeyEvents();

	}
}