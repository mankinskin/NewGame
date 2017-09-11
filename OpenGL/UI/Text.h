#pragma once
#include "..\Global\gl.h"
#include "GUI.h"



#define TEXT_LAYOUT_BOUND_LEFT 00000001
#define TEXT_LAYOUT_BOUND_RIGHT 00000011
#define TEXT_LAYOUT_CENTER 00000111
#define TEXT_LAYOUT_FORCE_X_FIT 00001000
#define TEXT_LAYOUT_FORCE_Y_FIT 00010000
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
					:thickness(1.5f), hardness(0.5f), front_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)), back_color(glm::vec4()) {}
				TextStyle(float pThickness, float pHardness, float pCr, float pCg, float pCb, float pCa)
					:thickness(pThickness), hardness(pHardness), front_color(glm::vec4(pCr, pCg, pCb, pCa)) {}
				TextStyle(float pThickness, float pHardness, glm::vec4 pFrontColor, glm::vec4 pBackColor = glm::vec4())
					:thickness(pThickness), hardness(pHardness), front_color(pFrontColor), back_color(pBackColor){}
				float thickness = 1.0f;
				float hardness = 1.0f;
				glm::vec4 front_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				glm::vec4 back_color = glm::vec4();
			};

			struct CharQuad {
				CharQuad() {}
				CharQuad(float pPosX, float pPosY, float pWidth, float pHeight)
					:posX(pPosX), posY(pPosY), width(pWidth), height(pHeight) {}
				float posX;
				float posY;
				float width;
				float height;
			};


			void setStringFont(unsigned int pStringIndex, unsigned int pFontIndex);
			void setStringStyle(unsigned int pStringIndex, unsigned int pStyleIndex);
			//Interface
			unsigned int createTextbox(unsigned int pPosIndex, unsigned int pSizeIndex, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging = 0.0f);
			unsigned int createTextboxMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale);
			void appendTextboxString(unsigned int pTextbox, unsigned int pStringIndex);
			void appendTextboxString(unsigned int pTextbox, String pString);
			void appendTextboxString(unsigned int pTextbox, std::string pString);
			
			//internal
			void loadTextboxGlyphs(Textbox& pTextbox);
			void translateTextbox(Textbox & pTextbox);
			void loadChars();
			void renderGlyphs();
			void revalidateTextboxStringIndices();
			
			void revalidateStringIndices();
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