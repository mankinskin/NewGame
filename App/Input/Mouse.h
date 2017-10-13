#pragma once
#include <glm/glm.hpp>
namespace App {
	namespace Input {
		extern glm::vec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		void updateCursor();
		void checkMouseEvents();
		void resetMouse();
		void toggleCursor();
	}
}