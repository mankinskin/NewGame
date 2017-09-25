#pragma once
#include <OpenGL\Global\gl.h>
#include <string>
namespace gl {
	namespace GUI {

		
		
		void setQuadVisibility(unsigned int pQuadIndex, int pHide);
                unsigned createQuad(glm::vec4 pQuad);
                unsigned createQuad(glm::vec2 pPos, glm::vec2 pSize);
		unsigned createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
                void reserveQuadSpace(unsigned int pCount);
		void clearBuffers();
		void initQuadBuffer();
		void updateQuadBuffer();
                extern std::vector<int> allQuadFlags;
		extern std::vector<glm::vec4> allQuads;
		extern unsigned int MAX_QUAD_COUNT;
		extern unsigned int quadBuffer;
		

	}
}
