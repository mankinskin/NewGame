#pragma once
#include <glm\glm.hpp>
#include <vector>

#define FONT_LOAD_DT 00000001

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
				Font() :kerningOffset(0), kerningCount(0), metricOffset(0), metricCount(0), glyphStorageIndex(0), atlasID(0), fontMetric(FontMetric()), stringOffset(0), stringCount(0) {}
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
			}; extern std::vector<FontInstructions> allFontInstructions;
			inline bool operator==(const FontInstructions& l, const FontInstructions& r) {
				return  l.pointSize == r.pointSize && l.startCode == r.startCode && l.glyphCount == r.glyphCount && l.flags == r.flags;
			}

			//Glyphs
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

			

			
			void initFontShader();
			void initFontVAO();
			void updateCharStorage();
			void clearCharStorage();

			extern unsigned int quadStorage;
			extern unsigned int charStorage;
			extern unsigned int glyphShapeProgram;
			extern unsigned int fontVAO;
			extern unsigned int MAX_CHARS;
			extern std::vector<Font> allFonts;
			extern std::vector<float> allKerning;
			extern std::vector<GlyphMetrics> allMetrics;
			
		}
	}
}