#pragma once
#include <OpenGL\Global\gl.h>
#include <string>
#include <unordered_map>
namespace gl {
	namespace GUI {
		unsigned int createColor(glm::vec4 pColor, std::string pColorName = "");
		void storeGUIColors();
		glm::vec4 & getQuad(unsigned int pID);
                void moveQuadByMouseDelta(unsigned int pQuadIndex);
		void moveQuad(unsigned int pQuadIndex, glm::vec2 pDelta);
                unsigned createQuad(glm::vec4 pQuad);
                unsigned createQuad(glm::vec2 pPos, glm::vec2 pSize);
		unsigned createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
                void reserveQuadSpace(unsigned int pCount);
		void clearBuffers();
		void initQuadBuffer();
		void updateQuadBuffer();
		void bindQuadParent(unsigned int pParentQuad, unsigned int pChildQuad);
		void updateElements();
	}
}
