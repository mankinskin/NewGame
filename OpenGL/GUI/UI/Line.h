#pragma once
#include <vector>
#include <glm\glm.hpp>
namespace gl {
	namespace GUI {
		unsigned int createLine(glm::vec2 pV1, glm::vec2 pV2, unsigned int pColorIndex = 0);
		void setLineVertex(unsigned int pLineIndex, unsigned int pVertexOffset, glm::vec2 pVertex);
		void setLineColor(unsigned int pLineIndex, unsigned int pColorIndex);
		void initLineVAO();
		void renderLines();
		void initLineShader();
		void updateLines();
		void setupLineShader();
		void bindLineParent(unsigned int pParentQuad, unsigned int pChildLine);
	}
}