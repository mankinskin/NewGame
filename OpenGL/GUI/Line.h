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
		extern std::vector<glm::vec4> allLines;
		extern std::vector<unsigned int> allLineColorIndices;
		
		extern unsigned int guiLineVAO;
		extern unsigned int guiLineVBO;
		extern unsigned int guiLineColorIndexVBO;
		extern unsigned int guiLineShader;
		extern unsigned int MAX_GUI_LINES;
	}
}