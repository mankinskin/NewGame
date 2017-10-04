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
			//Textboxes
			struct String {
				String()
					:offset(0), count(0){}
				String(unsigned int pOffset, unsigned int pCount)
					:offset(pOffset), count(pCount){}
				String(std::string pString);
				unsigned int offset;
				unsigned int count;
			};

			struct TextboxMetrics {
				TextboxMetrics(unsigned int pFont, glm::vec2 pGlyphScale, float pAdvanceScale, float pLineGapScale)
					:font(pFont), glyphScale(pGlyphScale), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
				TextboxMetrics(unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
					:font(pFont), glyphScale(glm::vec2(pGlyphScaleX, pGlyphScaleY)), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
				unsigned int font;
				glm::vec2 glyphScale;
				float advanceScale;
				float lineGapScale;
			};

			
			struct Textbox {
				Textbox()
					:metrics(0), chars(String()), pos(0), size(0), marging(0.0f), flags(0) {}
				
				String chars;
				unsigned int metrics;
				unsigned int pos;
				unsigned int size;
				float marging = 0.0f;
				unsigned int flags;
			};
			extern std::vector<glm::vec2> allTextboxPositions;
			extern std::vector<glm::vec2> allTextboxSizes;
			//Strings
			struct TextStyle {
				TextStyle()
					:thickness(1.5f), hardness(0.5f) {}
				TextStyle(float pThickness, float pHardness)
					:thickness(pThickness), hardness(pHardness) {}
				
				float thickness = 1.0f;
				float hardness = 1.0f;
				glm::vec2 pad;
			};

			struct TextColor {
				TextColor()
					:color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {}
				TextColor(float r, float g, float b, float a)
					:color(glm::vec4(r, g, b, a)) {}
				TextColor(glm::vec4& pColor)
					:color(pColor) {}
				glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			};

			struct CharQuad {
				CharQuad() {}
				CharQuad(float pPosX, float pPosY, float pWidth, float pHeight)
					:pos(glm::vec2(pPosX, pPosY)), size(glm::vec2(pWidth, pHeight)) {}
				glm::vec2 pos;
				glm::vec2 size;
			};

			struct TextboxGlyphs {
				TextboxGlyphs(unsigned int pGlyphCount) {
					quads.resize(pGlyphCount);
					glyphIndices.resize(pGlyphCount);
					lines.reserve(pGlyphCount);
					line_sizes.reserve(pGlyphCount);
				}
				std::vector<String> lines;
				std::vector<glm::vec2> line_sizes;
				std::vector<unsigned int> glyphIndices;
				std::vector<CharQuad> quads;
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
			
			//internal
			void parseStringFormat(String pString);
			void loadTextboxGlyphs(Textbox& pTextbox, TextboxMetrics& pTextMetrics, Font& pFont, TextboxGlyphs& pGlyphs);
			void transformTextboxGlyphs(Textbox& pTextbox, TextboxGlyphs& pGlyphs);
			void loadTextboxes();
			void renderGlyphs();
			void revalidateTextboxCharIndices();
			void revalidateFontStringIndices();
			void insertFontString(Font & pFont, String pString);
			
			
			unsigned int createTextStyle(TextStyle& pInstructions);
			unsigned int createTextStyle(float pThickness, float pHardness);
			unsigned int createTextColor(glm::vec4 pColor);
			void initStyleBuffer();
			extern unsigned int styleStorage;
			extern std::vector<TextStyle> allTextStyles;
			extern std::vector<glm::vec4> allTextColors;
			//extern std::vector<std::string> allTextColorNames;//only an idea. like white, grey, blue, pink, magenta
			extern std::vector<Textbox> allTextboxes;
			extern std::vector<String> allFontStrings;
			extern std::vector<unsigned int> textboxStringIndices;
			extern std::vector<String> allStrings;
			extern std::vector<unsigned char> allChars;
			extern std::vector<TextboxMetrics> allTextboxMetrics;
			extern std::vector<CharQuad> charQuadBuffer;
			extern std::vector<unsigned int> glyphIndexBuffer;
		}
	}
	
}