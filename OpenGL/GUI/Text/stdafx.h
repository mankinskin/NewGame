#pragma once
#include <vector>
namespace gl {
    namespace GUI {
	namespace Text {
	    struct FontMetric {
		FontMetric() :lineGap(0.0f), underline_drop(0.0f), underline_thickness(0.0f) {}
		float lineGap;
		float underline_drop;
		float underline_thickness;
	    };

	    struct Font {
		Font() :instructions(0), kerningOffset(0), kerningCount(0), metricOffset(0), metricCount(0), glyphStorageIndex(0), atlasID(0), fontMetric(FontMetric()), stringOffset(0), stringCount(0) {}
		size_t atlasID;
		FontMetric fontMetric;
		size_t instructions;
		size_t glyphStorageIndex;

		size_t kerningOffset;
		size_t kerningCount;
		size_t metricOffset;
		size_t metricCount;

		size_t stringOffset;
		size_t stringCount;
	    };


	    struct Glyph {
		Glyph() {  }
		Glyph(float minX, float minY, float maxX, float maxY)
		    :min(glm::vec2(minX, minY)), max(glm::vec2(maxX, maxY)) {}
		glm::vec2 min;
		glm::vec2 max;
	    };

	    struct GlyphMetrics {
		GlyphMetrics() {}
		GlyphMetrics(float pWidth, float pHeight, float pAdvanceX, float pBearingX, float pBearingY)
		    :width(pWidth), height(pHeight), advanceX(pAdvanceX), bearingX(pBearingX), bearingY(pBearingY) {}
		//in screen relative coordinates
		float width;
		float height;
		float advanceX;
		float bearingX;
		float bearingY;
	    };
	    struct FontInstructions {
		FontInstructions()
		    :pointSize(11), startCode(35), glyphCount(100), flags(0) {}
		FontInstructions(size_t pPointSize, size_t pStartCode, size_t pGlyphCount, int pFlags, size_t pUpsampling)
		    :pointSize(pPointSize), startCode(pStartCode), glyphCount(pGlyphCount), flags(pFlags), upsampling(pUpsampling) {}
		size_t startCode = 35;
		size_t glyphCount = 100;
		size_t pointSize = 11;
		int flags = 0;
		size_t upsampling = 4;
	    };

	    inline bool operator==(const FontInstructions& l, const FontInstructions& r) {
		return  l.pointSize == r.pointSize && l.startCode == r.startCode && l.glyphCount == r.glyphCount && l.flags == r.flags;
	    }
	    struct String {
		String()
		    :offset(0), count(0) {}
		String(size_t pOffset, size_t pCount)
		    :offset(pOffset), count(pCount) {}
		String(std::string pString);
		size_t offset;
		size_t count;
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
	    extern size_t fontVAO;
	    extern size_t glyphShaderProgram;
	    extern std::vector<size_t> glyphIndexBuffer;
	    extern std::vector<Font> allFonts;

	    extern std::vector<FontInstructions> allFontInstructions;
	    extern std::vector<GlyphMetrics> allMetrics;
	    extern std::vector<float> allKerning;
	    extern std::vector<String> allFontStrings;
	    extern std::vector<CharQuad> charQuadBuffer;
	    extern std::vector<unsigned char> allChars;
	}
    }
}