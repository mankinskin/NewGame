#ifndef FONT_LOADER_H
#define FONT_LOADER_H

#include <string>
#include <vector>
#include "TTF.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm\glm.hpp>
#include "Text.h"

namespace gl {
	namespace GUI {
		namespace Text {
			namespace Initializer {

				struct LoadFontMetrics {
					unsigned int ppem_x;
					unsigned int ppem_y;
					unsigned int scale_x;
					unsigned int scale_y;
					unsigned int lineGap;
				};

				struct LoadGlyphQuad {
					unsigned int minX;
					unsigned int minY;
					unsigned int maxX;
					unsigned int maxY;
				};

				struct LoadGlyphMetrics {
					unsigned int advanceX;
					int xBearing;
					int yBearing;
				};

				struct LoadAtlas {
					unsigned int width;
					unsigned int height;

					std::vector<LoadGlyphQuad> quads;
					std::vector<unsigned char> buffer;
				};

				struct LoadFont {
					std::string fileName;
					FontInstructions instructions;
					LoadFontMetrics fontMetrics;
					LoadAtlas atlas;
					std::vector<LoadGlyphMetrics> metrics;
					std::vector<int> kerningMap;
				};

				unsigned int includeFont(std::string pFontFileName, FontInstructions& pInstructions = FontInstructions());
				unsigned int includeFont(std::string pFontFileName, unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling = 1);
				unsigned int createFontInstructions(unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling = 1);
				unsigned int createFontInstructions(FontInstructions& pInstructions);
				
				extern int showDistanceField;

				//Font loading variables
				extern FT_Library ftLib;

				extern std::vector<LoadFont> loadFontBuffer;
				extern std::string font_input_directory;
				extern std::string font_store_directory;
				//Font loading helper functions
				std::vector<int> getKerningMap(FT_Face pFace, unsigned int pStartCode, unsigned int pGlyphCount);
				void loadAtlas(FT_Face& pFace, FontInstructions& pFontInstructions, LoadFontMetrics& pFontMetrics, LoadAtlas& pAtlas, std::vector<LoadGlyphMetrics>& pMetrics);

				//void convertAtlasToDistanceField();

				void storeGlyphs(Font& pFont, const LoadFont & pLoadFont);
				std::pair<unsigned int, unsigned int> convertKerning(std::vector<int>& pKerningMap);
				void setFontSize(FT_Face& pFace, LoadFontMetrics& pFontMetrics, FontInstructions& pInstructions);
				void removeFileExtension(std::string& pFileName);
				//Font loading instructions
				void loadFont(LoadFont& pFont);
				void initFreeType();
				
				void loadFonts();
				Font integrateFont(LoadFont & pFont);
				void setFontInputDir(std::string pNewDirectory);
				void setFontStoreDir(std::string pNewDirectory);
				void saveFontFile(std::string pFileName, LoadFont* pFont);
				int loadFontFile(std::string pFilePath, LoadFont* pFont);
			}
		}
	}
}



#endif