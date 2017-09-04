#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <OpenGL\UI\GUI.h>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>
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

		extern std::vector<unsigned> allSignals;
		extern unsigned TOTAL_SIGNAL_COUNT;
		
		template<class EventType>
		class Signal {
		public:
			Signal(unsigned pIndex, EventType pEvent):index(pIndex), evnt(pEvent){}
			EventType evnt;
			unsigned index;
		};

		class KeyEvent {
		public:
			KeyEvent()
				:key(-1), change(KeyCondition()) {}
			KeyEvent(Key pKey, KeyCondition pChange)
				:key(pKey), change(pChange){}
			KeyEvent(Key pKey, int pAction, int pMods)
				:key(pKey), change(KeyCondition(pAction, pMods)) {}

			Key key;
			KeyCondition change;
		};
		extern std::vector<Signal<KeyEvent>> allKeySignals;
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
		class FuncSlot {
			
		public:
			FuncSlot(): fun(nullptr), args(std::tuple<Args...>()), signal_bindings(std::vector<unsigned>())
			{}

			FuncSlot(R(*pF)(Args...) , Args... pArgs)
				:fun(pF), args(std::forward_as_tuple(pArgs)...), signal_bindings(), slot_index(instances.size()){
				
				instances.push_back(*this);
			}

			static void reserve_slots(unsigned int pCount) {
				instances.reserve(pCount);
			}
			static void invoke_all(){
				for (FuncSlot<R, Args...>& inst : instances) {
					if (inst.should_call()) {
						inst.invoke();
					}
				}
			}
			static void bind(unsigned int pSlot, std::initializer_list<unsigned int> pSignals);

			R invoke() const {
				return std::apply(fun, args);
			}
			R callFunc(Args... pArgs) const {
				return fun(pArgs...);
			}
		static std::vector<FuncSlot<R, Args...>> instances;
		private:
			R(*fun)(Args...);
			std::tuple<Args...> args;
			std::vector<unsigned int> signal_bindings;
			unsigned int slot_index;
			
			
			int should_call() {
				for (unsigned s : signal_bindings) {
					if (allSignals[s]) {
						return 1;
					}
				}
				return 0;					
			}
		};
		template<typename R, typename... Args>
		std::vector<FuncSlot<R, Args...>> FuncSlot<R, Args...>::instances = std::vector<FuncSlot<R, Args...>>();
		
		
		
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
		void toggleCenterCursorRef(int* pMode);
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

		template<typename R, typename ...Args>
		inline void FuncSlot<R, Args...>::bind(unsigned int pSlot, std::initializer_list<unsigned int> pSignals)
		{
			instances[pSlot].signal_bindings = std::vector<unsigned>(pSignals);
		}

}
}