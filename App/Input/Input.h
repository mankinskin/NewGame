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
		
		void init();
		void fetchGLFWEvents();
		void checkEvents();
                void end();
		
		static void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
		static void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
		static void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
		static void cursorEnter_Callback(GLFWwindow * window, int pEntered);
		static void mouseKey_Callback(GLFWwindow* window, int pKey, int pAction, int pMods);
		static void scroll_Callback(GLFWwindow* window, double pX, double pY);
        }
}
