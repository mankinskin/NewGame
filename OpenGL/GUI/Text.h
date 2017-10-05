#pragma once
#include "..\Global\gl.h"
#include "GUI.h"
#include "Font.h"

#define TEXT_LAYOUT_BOUND_LEFT 00000001
#define TEXT_LAYOUT_BOUND_RIGHT 00000010
#define TEXT_LAYOUT_CENTER_X 00001000
#define TEXT_LAYOUT_CENTER_Y 00000100
#define TEXT_LAYOUT_CENTER_BOTH 00001100
#define TEXT_LAYOUT_FREE_LINES 00010000

namespace gl {
	namespace GUI {
		namespace Text {
			
			struct String {
				String()
					:offset(0), count(0) {}
				String(unsigned int pOffset, unsigned int pCount)
					:offset(pOffset), count(pCount) {}
				String(std::string pString);
				unsigned int offset;
				unsigned int count;
			};

			struct TextStyle {
				TextStyle()
					:thickness(1.5f), hardness(0.5f) {}
				TextStyle(float pThickness, float pHardness)
					:thickness(pThickness), hardness(pHardness) {}

				float thickness = 1.0f;
				float hardness = 1.0f;
				glm::vec2 pad;
			};
			struct CharQuad {
				CharQuad() {}
				CharQuad(float pPosX, float pPosY, float pWidth, float pHeight)
					:pos(glm::vec2(pPosX, pPosY)), size(glm::vec2(pWidth, pHeight)) {}
				glm::vec2 pos;
				glm::vec2 size;
			};
			void setStringColor(unsigned int pStringIndex, unsigned int pColorIndex);
			void setStringStyle(unsigned int pStringIndex, unsigned int pStyleIndex);
			//Interface
			void reserveTextboxSpace(unsigned int pCount);
			unsigned int createTextbox(unsigned int pQuadIndex, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextbox(unsigned int pPosIndex, unsigned int pSizeIndex, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
                        unsigned int createTextbox(glm::vec4 pQuad, unsigned int pMetrics, int pFlags, float pMarging);
			unsigned int createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextboxMetrics(unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale);
			void setTextboxString(unsigned int pTextbox, std::string pString);

			void setTextboxString(unsigned int pTextbox, String pString);
			
			void loadTextboxes();
			void renderGlyphs();
			void revalidateTextboxCharIndices();
			
			void insertFontString(Font & pFont, String pString);
			
			
			unsigned int createTextStyle(TextStyle& pInstructions);
			unsigned int createTextStyle(float pThickness, float pHardness);
			unsigned int createTextColor(glm::vec4 pColor);
			void initStyleBuffer();
			//extern std::vector<std::string> allTextColorNames;//only an idea. like white, grey, blue, pink, magenta
			
			extern std::vector<String> allFontStrings;
			extern std::vector<CharQuad> charQuadBuffer;
			extern std::vector<unsigned char> allChars;
		}
	}
	
}