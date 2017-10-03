#pragma once
#include <OpenGL\Global\gl.h>
#include <string>
#include <unordered_map>
namespace gl {
	namespace GUI {
		unsigned int createColor(glm::vec4 pColor, std::string pColorName = "");
		void storeGUIColors();
                void moveQuadByMouseDelta(unsigned int pQuadIndex);
		void moveQuadGroupByMouseDelta(unsigned int GroupIndex);
		void setQuadVisibility(unsigned int pQuadIndex, int pHide);
                unsigned createQuad(glm::vec4 pQuad);
                unsigned createQuad(glm::vec2 pPos, glm::vec2 pSize);
		unsigned createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
                void reserveQuadSpace(unsigned int pCount);
		void clearBuffers();
		void initQuadBuffer();
		void updateQuadBuffer();
		extern std::vector<glm::vec4> allColors;
		extern std::unordered_map<std::string, unsigned int> colorLookup;
                extern std::vector<int> allQuadFlags;
		extern std::vector<glm::vec4> allQuads;
		extern unsigned int MAX_QUAD_COUNT;
		extern unsigned int quadBuffer;
		extern unsigned int colorBuffer;

	}
}
