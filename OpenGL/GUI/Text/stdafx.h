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
				unsigned int atlasID;
				FontMetric fontMetric;
				unsigned int instructions;
				unsigned int glyphStorageIndex;

				unsigned int kerningOffset;
				unsigned int kerningCount;
				unsigned int metricOffset;
				unsigned int metricCount;

				unsigned int stringOffset;
				unsigned int stringCount;
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
				FontInstructions(unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling)
					:pointSize(pPointSize), startCode(pStartCode), glyphCount(pGlyphCount), flags(pFlags), upsampling(pUpsampling) {}
				unsigned int startCode = 35;
				unsigned int glyphCount = 100;
				unsigned int pointSize = 11;
				int flags = 0;
				unsigned int upsampling = 4;
			};

			inline bool operator==(const FontInstructions& l, const FontInstructions& r) {
				return  l.pointSize == r.pointSize && l.startCode == r.startCode && l.glyphCount == r.glyphCount && l.flags == r.flags;
			}
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
			extern unsigned int fontVAO;
			extern unsigned int glyphShaderProgram;
			extern std::vector<unsigned int> glyphIndexBuffer;
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