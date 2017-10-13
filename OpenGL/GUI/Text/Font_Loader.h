#ifndef FONT_LOADER_H
#define FONT_LOADER_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm\glm.hpp>
#include <string>
namespace gl {
    namespace GUI {
	namespace Text {
	    struct FontInstructions;
	    namespace Initializer {

		void initFreeType();
		size_t includeFont(std::string pFontFileName, FontInstructions& pInstructions);
		size_t includeFont(std::string pFontFileName, size_t pPointSize, size_t pStartCode, size_t pGlyphCount, int pFlags, size_t pUpsampling = 1);
		size_t createFontInstructions(size_t pPointSize, size_t pStartCode, size_t pGlyphCount, int pFlags, size_t pUpsampling = 1);
		size_t createFontInstructions(FontInstructions& pInstructions);

		void loadFonts();
		void setFontInputDir(std::string pNewDirectory);
		void setFontStoreDir(std::string pNewDirectory);
	    }
	}
    }
}



#endif