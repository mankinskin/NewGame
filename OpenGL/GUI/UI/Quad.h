#pragma once
#include <glm/glm.hpp>
namespace gl {
	namespace GUI {
		struct Quad {
			Quad(glm::vec4 pQuad)
				:pos(glm::vec2(pQuad.x, pQuad.y)), size(glm::vec2(pQuad.z, pQuad.w)) {}
			Quad(glm::vec2 pPos, glm::vec2 pSize)
				:pos(pPos), size(pSize){}
			glm::vec2 pos;
			glm::vec2 size;
		};

		Quad & getQuad(size_t pID);
		size_t createQuad(glm::vec4 pQuad);
		size_t createQuad(glm::vec2 pPos, glm::vec2 pSize);
		size_t createQuad(Quad & pQuad);
		size_t createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
		void reserveQuadSpace(size_t pCount);
		void clearBuffers();
		void initQuadBuffer();
		void updateQuadBuffer();
	}
}