#pragma once
#include "..\Global\gl.h"
#include "GUI.h"



#define TEXT_LAYOUT_BOUND_LEFT 00000001
#define TEXT_LAYOUT_BOUND_RIGHT 00000010
#define TEXT_LAYOUT_CENTER_X 00001000
#define TEXT_LAYOUT_CENTER_Y 00000100
#define TEXT_LAYOUT_CENTER_BOTH 00001100
namespace gl {
	namespace GUI {
		namespace Text {
			//Textboxes
			struct String {
				String()
					:charOffset(0), charCount(0), font(0), style(0){}
				String(unsigned int pOffset, unsigned int pCount, unsigned int pFont = 0, unsigned int pStyle = 0)
					:charOffset(pOffset), charCount(pCount), font(pFont), style(pStyle) {}
				String(std::string pString, unsigned int pFont = 0, unsigned int pStyle = 0);
				unsigned int charOffset;
				unsigned int charCount;
				unsigned int font;
				unsigned int style;
			};

			struct TextboxMetrics {
				TextboxMetrics(glm::vec2 pGlyphScale, float pAdvanceScale, float pLineGapScale)
					:glyphScale(pGlyphScale), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
				TextboxMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
					:glyphScale(glm::vec2(pGlyphScaleX, pGlyphScaleY)), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
				glm::vec2 glyphScale;
				float advanceScale;
				float lineGapScale;
			};

			
			struct Textbox {
				Textbox()
					:stringOffset(0), stringCount(0), pos(0), size(0), marging(0.0f), flags(0) {}
				
				unsigned int stringOffset;
				unsigned int stringCount;
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
					:thickness(1.5f), hardness(0.5f), front_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {}
				TextStyle(float pThickness, float pHardness, float pCr, float pCg, float pCb, float pCa)
					:thickness(pThickness), hardness(pHardness), front_color(glm::vec4(pCr, pCg, pCb, pCa)) {}
				TextStyle(float pThickness, float pHardness, glm::vec4 pFrontColor, glm::vec4 pBackColor = glm::vec4())
					:thickness(pThickness), hardness(pHardness), front_color(pFrontColor){}
				glm::vec4 front_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

			struct TextboxGlyphs {
				TextboxGlyphs(unsigned int pGlyphCount) {
					quads.resize(pGlyphCount);
					glyphIndices.resize(pGlyphCount);
				}
				std::vector<String> lines;
				std::vector<glm::vec2> line_sizes;
				std::vector<unsigned int> glyphIndices;
				std::vector<CharQuad> quads;
			};

			void setStringFont(unsigned int pStringIndex, unsigned int pFontIndex);
			void setStringStyle(unsigned int pStringIndex, unsigned int pStyleIndex);
			//Interface
			void reserveTextboxSpace(unsigned int pCount);
			unsigned int createTextbox(unsigned int pQuadIndex, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextbox(unsigned int pPosIndex, unsigned int pSizeIndex, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextboxMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale);
			void appendTextboxString(unsigned int pTextbox, unsigned int pStringIndex);
			void appendTextboxString(unsigned int pTextbox, String pString);
			void appendTextboxString(unsigned int pTextbox, std::string pString);
			
			//internal
			void loadTextboxGlyphs(Textbox& pTextbox, TextboxGlyphs& pGlyphs);
			void transformTextboxGlyphs(Textbox& pTextbox, TextboxGlyphs& pGlyphs);
			void loadChars();
			void renderGlyphs();
			void revalidateTextboxStringIndices();
			
			
			unsigned int createTextStyle(TextStyle& pInstructions);
			unsigned int createTextStyle(float pThickness, float pHardness, glm::vec4 pFrontColor, glm::vec4 pBackColor = glm::vec4());

			void initStyleBuffer();
			extern unsigned int styleStorage;
			extern std::vector<TextStyle> allTextStyles;
			extern std::vector<Textbox> allTextboxes;
			extern std::vector<unsigned int> fontStringIndices;
			extern std::vector<unsigned int> textboxStringIndices;
			extern std::vector<String> allStrings;
			extern std::vector<unsigned char> allChars;
			extern std::vector<TextboxMetrics> allTextboxMetrics;
			extern std::vector<CharQuad> charQuadBuffer;
			extern std::vector<unsigned int> glyphIndexBuffer;
		}
	}
	
}