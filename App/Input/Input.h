#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
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
		
		extern std::vector<unsigned int> rejectedSignals;
		extern std::vector<unsigned int> signalBuffer; //signal indices to trigger if not locked
		
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
