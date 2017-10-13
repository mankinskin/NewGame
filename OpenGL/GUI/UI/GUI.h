#pragma once
#include <OpenGL\Global\gl.h>
#include <string>
#include <unordered_map>
namespace gl {
	namespace GUI {
		void init();
		size_t createColor(glm::vec4 pColor, std::string pColorName = "");
		void storeGUIColors();
	}
}
