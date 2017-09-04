#pragma once
#include "..\Global\gl.h"
#include "GUI.h"

#define FONT_LOAD_DT 00000001

#define TEXT_LAYOUT_BOUND_LEFT 00000001
#define TEXT_LAYOUT_BOUND_RIGHT 00000011
#define TEXT_LAYOUT_CENTER 00000111
#define TEXT_LAYOUT_FORCE_X_FIT 00001000
#define TEXT_LAYOUT_FORCE_Y_FIT 00010000
namespace gl {
	namespace GUI {
		namespace Text {

			//Textboxes

			struct TextMetrics {
				TextMetrics(glm::vec2 pGlyphScale, float pAdvanceScale, float pLineGapScale)
					:glyphScale(pGlyphScale), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
				TextMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
					:glyphScale(glm::vec2(pGlyphScaleX, pGlyphScaleY)), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}

				glm::vec2 glyphScale;
				float advanceScale;
				float lineGapScale;
			};

			struct Textbox {
				Textbox()
					:stringOffset(0), stringCount(0), flags(0), pos(nullptr), size(nullptr), marging(0.0f) {}

				unsigned int stringOffset = 0;
				unsigned int stringCount = 0;
				TextMetrics* metrics;
				glm::vec2* pos;
				glm::vec2* size;
				float marging = 0.0f;
				unsigned int flags;
			};
			extern std::vector<Textbox> allTextboxes;
			extern std::vector<TextMetrics> allTextMetrics;

			//Strings

			struct StringRenderInstructions {
				StringRenderInstructions()
					:thickness(1.5f), hardness(0.5f), depth(0.0f), color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {}
				StringRenderInstructions(float pThickness, float pHardness, float pDepth, float pCr, float pCg, float pCb, float pCa)
					:thickness(pThickness), hardness(pHardness), depth(pDepth), color(glm::vec4(pCr, pCg, pCb, pCa)) {}
				StringRenderInstructions(float pThickness, float pHardness, float pDepth, glm::vec4 pColor)
					:thickness(pThickness), hardness(pHardness), depth(pDepth), color(pColor) {}

				float thickness = 1.0f;
				float hardness = 1.0f;
				float depth = 0.0f;
				glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			};
			StringRenderInstructions* createStringRenderInstructions(StringRenderInstructions& pInstructions);
			StringRenderInstructions* createStringRenderInstructions(float pThickness, float pHardness, float pDepth, glm::vec4 pColor);
			
			extern std::vector<StringRenderInstructions> allStrRenderInstructions;

			struct String {
				String()
					:charOffset(0), charCount(0), render_instructions(nullptr) {}
				String(unsigned int pOffset, unsigned int pCount, StringRenderInstructions* pInstructions)
					:charOffset(pOffset), charCount(pCount), render_instructions(pInstructions){}
				unsigned int charOffset;
				unsigned int charCount;
				StringRenderInstructions* render_instructions;
			};
			struct LoadString {
				LoadString()
					:string(String()),  fontIndex(-1) {}
				LoadString(String pString, unsigned int pFontIndex)
					:string(pString), fontIndex(pFontIndex) {}

				String string;				
				unsigned int fontIndex;
				Textbox* textbox;
			};

			//Fonts
			struct FontMetric {
				FontMetric() :lineGap(0.0f), underline_drop(0.0f), underline_thickness(0.0f) {}
				float lineGap;
				float underline_drop;
				float underline_thickness;
			};

			struct Font {
				Font() :stringOffset(0), stringCount(0), kerningOffset(0), kerningCount(0), metricOffset(0), metricCount(0), glyphStorageIndex(0), atlasID(0), fontMetric(FontMetric()){}
				unsigned int atlasID;
				FontMetric fontMetric;
				unsigned int instructions;
				unsigned int glyphStorageIndex;

				unsigned int kerningOffset;
				unsigned int kerningCount;
				unsigned int metricOffset;
				unsigned int metricCount;
				unsigned int stringOffset = 0;
				unsigned int stringCount = 0;
			};
			struct FontInstructions {
				FontInstructions()
					:pointSize(11), startCode(35), glyphCount(100), flags(0) {}
				FontInstructions(unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling)
					:pointSize(pPointSize), startCode(pStartCode), glyphCount(pGlyphCount),  flags(pFlags) , upsampling(pUpsampling){}
				unsigned int startCode = 35;
				unsigned int glyphCount = 100;
				unsigned int pointSize = 11;
				int flags = 0;
				unsigned int upsampling = 4;
			};extern std::vector<FontInstructions> allFontInstructions;
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
		
			struct CharQuad {
				CharQuad() {}
				CharQuad(float pPosX, float pPosY, float pWidth, float pHeight)
					:posX(pPosX), posY(pPosY), width(pWidth), height(pHeight) {}
				float posX;
				float posY;
				float width;
				float height;
			};

			void renderGlyphs();
			void initCharStorage();
			void initFontShader();
			void initFontVAO();
			void insertTextboxString(Textbox* pTextbox, LoadString pString);
			LoadString createString(std::string pChars, StringRenderInstructions* pInstructions = nullptr, unsigned int pFontIndex = -1);
			Textbox* createTextbox(Quad pQuad, TextMetrics * pMetrics, int pFlags, float pMarging = 0.0f);
			Textbox* createTextbox(glm::vec2 * pTopLeft, glm::vec2 * pSize, TextMetrics * pMetrics, int pFlags, float pMarging = 0.0f);
			TextMetrics * createTextMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale);
			void loadStrings();
			String loadString(LoadString& pString);
			void updateCharStorage();
			void clearCharStorage();

			extern unsigned int fontCount;
			extern std::vector<Font> allFonts;
			extern std::vector<float> allKerning;
			extern std::vector<GlyphMetrics> allMetrics;
			extern std::vector<CharQuad> charQuadBuffer;
			extern std::vector<unsigned int> glyphIndexBuffer;
			extern std::vector<unsigned char> charBuffer;
			extern std::vector<LoadString> loadStringBuffer;
			extern std::vector<String> stringBuffer;
			extern unsigned int MAX_CHARS;
			extern unsigned int quadStorage;
			extern unsigned int charStorage;
			extern unsigned int glyphShapeProgram;
			extern unsigned int fontVAO;
		}
	}
}