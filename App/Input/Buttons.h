#pragma once
#include "Input.h"

namespace App {
	namespace Input {
			using Button = unsigned int;

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

			class ButtonEvent {
			public:
				ButtonEvent()
					:button(-1), change(ButtonCondition()) {}
				ButtonEvent(Button pButton, ButtonCondition pChange)
					:button(pButton), change(pChange) {}
				ButtonEvent(Button pButton, unsigned pAction, unsigned pIn)
					:button(pButton), change(ButtonCondition(pAction, pIn)) {}

				Button button;
				ButtonCondition change;
			};
			extern std::vector<EventSlot<ButtonEvent>> allButtonSignals;
			inline bool operator==(ButtonEvent const & l, ButtonEvent const& r) {
				return l.button == r.button && l.change == r.change;
			}
			extern std::vector<ButtonEvent> buttonEventBuffer;
			//Buttons
			extern std::vector<ButtonCondition> allButtonStates;
			extern std::vector<glm::vec4> allDetectors;
			
			inline bool is_inside_quad(glm::vec2& pPoint, glm::vec2& pTopLeft, glm::vec2& pLowerRight) {
				return (pPoint.x > pTopLeft.x) && (pPoint.x < pLowerRight.x) &&
					(pPoint.y < pTopLeft.y) && (pPoint.y > pLowerRight.y);
			}
			inline bool is_inside_quad(glm::vec2& pPoint, glm::vec4& pQuad) {
				return is_inside_quad(pPoint, glm::vec2(pQuad.x, pQuad.y), glm::vec2(pQuad.z, pQuad.w));
			}
			void loadButtons();
			void clearButtons();
			void fetchButtonEvents();
			void checkButtonEvents();
			
			unsigned int addButton(unsigned int pQuadIndex);
			void hideButton(unsigned int pButtonIndex);
			void unhideButton(unsigned int pButtonIndex);
			void toggleButton(unsigned int pButtonIndex);
			extern std::vector<int> allButtonFlags;
			extern std::vector<unsigned int> allButtonQuads;
	}
}