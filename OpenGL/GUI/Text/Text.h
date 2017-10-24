#pragma once
#include "..\..\Global\gl.h"

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
	    struct Font;
	    struct String;
	    struct TextStyle;
	    void setStringColor(size_t pStringIndex, size_t pColorIndex);
	    void setStringStyle(size_t pStringIndex, size_t pStyleIndex);
	    //Interface
	    void reserveTextboxSpace(size_t pCount);
	    size_t createTextbox(size_t pQuadIndex, size_t pMetrics, int pFlags, float pMarging = 0.0f);
	    size_t createTextbox(size_t pPosIndex, size_t pSizeIndex, size_t pMetrics, int pFlags, float pMarging = 0.0f);
	    size_t createTextbox(glm::vec4 pQuad, size_t pMetrics, int pFlags, float pMarging);
	    size_t createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, size_t pMetrics, int pFlags, float pMarging = 0.0f);
	    size_t createTextboxMetrics(size_t pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale);
	    void setTextboxString(size_t pTextbox, std::string pString);

	    void setTextboxString(size_t pTextbox, String pString);

	    void loadTextboxes();
	    void renderGlyphs();
	    void revalidateTextboxCharIndices();

	    void insertFontString(Font & pFont, String pString);
	    void setupGlyphShader();

	    size_t createTextStyle(TextStyle& pInstructions);
	    size_t createTextStyle(float pThickness, float pHardness);
	    size_t createTextColor(glm::vec4 pColor);
	    void initStyleBuffer();
	    //extern std::vector<std::string> allTextColorNames;//only an idea. like white, grey, blue, pink, magenta


	}
    }

}