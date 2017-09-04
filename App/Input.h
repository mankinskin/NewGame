#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <OpenGL\UI\GUI.h>
#include "managed_vector.h"
#include <functional>
#include <tuple>
#include <utility>
namespace App {
	namespace Input {

		using Key = int;
		using Detector = unsigned int;

		class ButtonCondition {
		public:
			ButtonCondition()
				:action(0), // 0 = release, 1 = press
				in(0) {}
			ButtonCondition(int pAction, int pIn)
				:action(pAction),
				in(pIn)
			{}
			int action;
			int in;
		};
		inline bool operator==(ButtonCondition const & l, ButtonCondition const& r) {
			return l.action == r.action && l.in == r.in;
		}
		inline bool operator!=(ButtonCondition const & l, ButtonCondition const& r) {
			return l.action != r.action || l.in != r.in;
		}

		class KeyCondition {
		public:
			KeyCondition()
				:action( 0), // 0 = release, 1 = press
				mods(0){}
			KeyCondition(int pAction, int pMods)
				:action(pAction),
				mods(pMods)
			{}
			KeyCondition(ButtonCondition pCondition)
				:action(pCondition.action),
				mods(pCondition.in)
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
				:key(pKey), change(pChange){}

			Key key;
			KeyCondition change;
		};
		extern std::vector<KeyEvent> keySignalBindings;
		extern std::vector<unsigned> signals;
		inline bool operator==(KeyEvent const & l, KeyEvent const& r) {
			return l.key == r.key && l.change == r.change;
		}

		class ButtonEvent{
		public:
			ButtonEvent()
				:button(-1), change(ButtonCondition()) {}
			ButtonEvent(Key pButton, ButtonCondition pChange)
				:button(pButton), change(pChange) {}

			Detector button;
			ButtonCondition change;
		};
		inline bool operator==(ButtonEvent const & l, ButtonEvent const& r) {
			return l.button == r.button && l.change == r.change;
		}

		extern std::vector<KeyEvent> keyEventBuffer;
		extern std::vector<ButtonEvent> buttonEventBuffer;

		
		template<typename R, typename... Args>
		class FuncBinding {
		private:
			std::tuple<Args...> args;
			R(*fun)(Args...);
		public:
			
			FuncBinding(R(*pF)(Args...) , Args... pArgs) 
				:fun(pF), args(std::forward<Args>(pArgs)...), signalBindings(std::vector<unsigned int>()){
				
			}
			std::vector<unsigned int> signalBindings;
			static std::vector<FuncBinding<R, Args...>> allInstances;


			void bind(unsigned int pSignalBinding) {
				//this check shouldnt be needed. initialize function bindings at function creation
				//auto it = std::find(signalBindings.begin(), signalBindings.end(), pSignalBinding);
				//if (it == signalBindings.end()) {
					signalBindings.push_back(pSignalBinding);
				//}
			}

			void fix() {
				allInstances.push_back(*this);
			}

			R invoke() {
				return std::apply(fun, args);
			}

			R callFunc(Args... pArgs) {
				return fun(pArgs...);
			}
		};

		template<typename R, typename... Args>
		std::vector<FuncBinding<R, Args...>> FuncBinding<R, Args...>::allInstances = std::vector<FuncBinding<R, Args...>>();
		
		extern std::vector<void(*)()> callbackBuffer;
		//Buttons
		extern std::vector<gl::GUI::Quad> quadLoadBuffer;
		extern std::vector<ButtonCondition> allButtonStates;
		extern std::vector<glm::vec4> allDetectors;
		//Keys
		//Mouse
		extern KeyCondition mouseButtons[3];
		extern glm::dvec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		extern int scroll;
		extern int centerCursor;
		
		void init();
		void frame_end();
		void callFunctions();
		void setCursorMode(int pMode);
		void toggleCenterCursor();
		void checkEvents();
		//KEYS


		//BUTTONS
		inline bool is_inside_quad(glm::vec2& pPoint, glm::vec2& pTopLeft, glm::vec2& pLowerRight) {
			return (pPoint.x > pTopLeft.x) && (pPoint.x < pLowerRight.x) &&
				(pPoint.y < pTopLeft.y) && (pPoint.y > pLowerRight.y);
		}
		inline bool is_inside_quad(glm::vec2& pPoint, glm::vec4& pQuad) {
			return is_inside_quad(pPoint, glm::vec2(pQuad.x, pQuad.y), glm::vec2(pQuad.z, pQuad.w));
		}
		void calculateDetectionRanges();
		void clearDetectionRanges();
		
		//GLFW base
		void fetchGLFWEvents();
		void fetchButtonEvents();
		static void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
		static void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
		static void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
		static void cursorEnter_Callback(GLFWwindow * window, int pEntered);
		static void mouseButton_Callback(GLFWwindow* window, int pButton, int pAction, int pMods);
		static void scroll_Callback(GLFWwindow* window, double pX, double pY);

	}
}