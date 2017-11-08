#pragma once
#include <glm/glm.hpp>
namespace gl {
	namespace GUI {

		size_t createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
		
		void reserveQuads(size_t pCount);
		void clearBuffers();
		void initQuadBuffer();
		void updateQuadBuffer();

	}
}