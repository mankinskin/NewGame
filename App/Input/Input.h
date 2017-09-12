#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <OpenGL\UI\GUI.h>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>
#include "Event.h"
namespace App {
	namespace Input {
		

		void init();
		void end();
		void callFunctions();
		
		void checkEvents();
		
		extern std::vector<void(*)()> callbackBuffer;
		
		extern std::vector<unsigned> allSignalSlots; // 1 = signaled
		extern std::vector<int> allSignalLocks; // 1 = locked 

		extern std::vector<unsigned int> rejectedSignals;
		extern std::vector<unsigned int> signalBuffer; //signal indices to trigger if not locked
		extern unsigned TOTAL_SIGNAL_COUNT;

		//this might be kinda slow. it should store an array of indices for each "LockSignal" which is a signal which either lock or unlock the array of other signals
		extern std::unordered_map<unsigned int, std::vector<unsigned int>> signalLockBindings;
		extern std::unordered_map<unsigned int, std::vector<unsigned int>> signalUnlockBindings;

		// set pLockSignal to set the locks of pTargetSignals to pLock
		static void signal_unlock(unsigned int pLockSignal, std::initializer_list<unsigned int> pTargetSignals) {
			auto it = signalUnlockBindings.find(pLockSignal);
			if (it == signalUnlockBindings.end()) {
				signalUnlockBindings.insert(std::pair<unsigned int, std::vector<unsigned int>>(pLockSignal, std::vector<unsigned int>(pTargetSignals)));
				return;
			}
			it->second.insert(it->second.end(), pTargetSignals.begin(), pTargetSignals.end());
		}
		static void signal_lock(unsigned int pLockSignal, std::initializer_list<unsigned int> pTargetSignals) {
			auto it = signalLockBindings.find(pLockSignal);
			if (it == signalLockBindings.end()) {
				signalLockBindings.insert(std::pair<unsigned int, std::vector<unsigned int>>(pLockSignal, std::vector<unsigned int>(pTargetSignals)));
				return;
			}
			it->second.insert(it->second.end(), pTargetSignals.begin(), pTargetSignals.end());
		}
		
		void clearSignals();
		void initMenuSignals();
		void initGameGUISignals();
		void fetchGLFWEvents();

		static void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
		static void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
		static void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
		static void cursorEnter_Callback(GLFWwindow * window, int pEntered);
		static void mouseButton_Callback(GLFWwindow* window, int pButton, int pAction, int pMods);
		static void scroll_Callback(GLFWwindow* window, double pX, double pY);


}
}