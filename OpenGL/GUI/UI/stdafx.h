#pragma once
#include <vector>
#include <unordered_map>
namespace gl {
	namespace GUI {
		extern std::vector<glm::vec4> allColors;
		extern std::vector<glm::vec4> allQuads;
		extern std::vector<glm::vec2> allQuadDeltas;
		extern std::vector<int> allQuadFlags;
		extern unsigned int MAX_QUAD_COUNT;
		extern unsigned int quadBuffer;
		extern unsigned int colorBuffer;
		extern std::unordered_map<std::string, unsigned int> colorLookup;
	}
}