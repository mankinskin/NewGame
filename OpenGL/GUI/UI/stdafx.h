#pragma once
#include <vector>
#include <unordered_map>
#include <glm\glm.hpp>
namespace gl {
	namespace GUI {
		struct Quad;
		//Colors
		extern std::vector<glm::vec4> allColors;
		extern size_t colorBuffer;
		extern std::unordered_map<std::string, size_t> colorLookup;
		extern std::vector<glm::vec4> allQuads;
		extern size_t MAX_QUAD_COUNT;
		extern size_t quadBuffer;
	}
}