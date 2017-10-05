#ifndef FONT_LOADER_H
#define FONT_LOADER_H


#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm\glm.hpp>
#include "Font.h"
#include <string>
#include <vector>
namespace gl {
	namespace GUI {
		namespace Text {
			namespace Initializer {

				void initFreeType();
				unsigned int includeFont(std::string pFontFileName, FontInstructions& pInstructions = FontInstructions());
				unsigned int includeFont(std::string pFontFileName, unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling = 1);
				unsigned int createFontInstructions(unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling = 1);
				unsigned int createFontInstructions(FontInstructions& pInstructions);
				
				void loadFonts();
				void setFontInputDir(std::string pNewDirectory);
				void setFontStoreDir(std::string pNewDirectory);
			}
		}
	}
}



#endif