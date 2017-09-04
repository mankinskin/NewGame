#include "..\Global\stdafx.h"
#include "Font_Loader.h"

#define _USE_MATH_DEFINES
#include "..\BaseGL\Shader.h"
#include "..\BaseGL\Texture.h"
#include "..\Global\glDebug.h"
#include "..\Camera.h"
#include "..\BaseGL\Shader_Data.h"
#include <App\Global\Debug.h>
#include <math.h>
#include <OpenGL\Utility\dt\dt.h>
#include <OpenGL\Utility\dt\image.h>
#include <fstream>
#include <App/ContextWindow.h>
#define DEFAULT_TTF_DIR "..//assets//fonts//"
#define DEFAULT_STORE_DIR "..//assets//glyph_atlas_fonts//"
std::vector<gl::GUI::Text::Initializer::LoadFont> gl::GUI::Text::Initializer::loadFontBuffer = {};
std::string gl::GUI::Text::Initializer::font_input_directory = DEFAULT_TTF_DIR;
std::string gl::GUI::Text::Initializer::font_store_directory = DEFAULT_STORE_DIR;
FT_Library gl::GUI::Text::Initializer::ftLib;
int gl::GUI::Text::Initializer::showDistanceField = 1;

/*
PIPELINE FOR FONT LOADING
- Include desired font paths 
	- set load instructions for each font
- loadFonts

*/
//template<typename T>
//inline T square(T p) {
//	return p*p;
//}
void gl::GUI::Text::Initializer::
loadFonts()
{
	
	unsigned int queueCount = loadFontBuffer.size();
	allFonts.resize(fontCount + queueCount);
	for (unsigned int f = 0; f < queueCount; ++f) {
		LoadFont load_font = loadFontBuffer[f];
		loadFont(load_font);
		Font font = integrateFont(load_font);
		allFonts[fontCount + f] = font;
	}
	fontCount += queueCount;
	loadFontBuffer.clear();
}

std::vector<int> gl::GUI::Text::Initializer::
getKerningMap(FT_Face pFace, unsigned int pStartCode, unsigned int pGlyphCount)
{
	std::vector<int> kerning(pGlyphCount*pGlyphCount);
	//Get Kerning
	if (FT_HAS_KERNING(pFace)) {
		for (unsigned int g = 0; g < pGlyphCount; ++g) {
			for (unsigned int t = 0; t < pGlyphCount; ++t) {
				FT_Vector vec = FT_Vector();
				FT_Get_Kerning(pFace, FT_Get_Char_Index(pFace, pStartCode + g), FT_Get_Char_Index(pFace, pStartCode + t), 0, &vec);
				if (vec.x) {
					vec.x = vec.x;
				}
				kerning[g*pGlyphCount + t] = (vec.x)/64;
			}
		}
	}
	return kerning;
}



void gl::GUI::Text::Initializer::
setFontSize(FT_Face& pFace, LoadFontMetrics& pFontMetrics, FontInstructions& pInstructions) {
	
	unsigned int dpi_x = App::ContextWindow::primaryMonitor.dpi_x;
	unsigned int dpi_y = App::ContextWindow::primaryMonitor.dpi_y;
	if (pInstructions.flags & FONT_LOAD_DT) {
		dpi_x *= pInstructions.upsampling;
		dpi_y *= pInstructions.upsampling;
	}
	FT_Set_Char_Size(pFace, pInstructions.pointSize << 6, pInstructions.pointSize << 6, dpi_x, 0);
	
	pFontMetrics.scale_x = pFace->size->metrics.x_scale;
	pFontMetrics.scale_y = pFace->size->metrics.y_scale;
	pFontMetrics.ppem_x = pFace->size->metrics.x_ppem;
	pFontMetrics.ppem_y = pFace->size->metrics.y_ppem;
	//pFontMetrics.lineGap = FT_MulFix(pFace->size->metrics.ascender - pFace->size->metrics.descender, pFace->size->metrics.y_scale)/64;
	
}


void gl::GUI::Text::Initializer::
loadFont(LoadFont& pFont)
{
	

	FT_Face ftface;
	std::string fontPath = font_input_directory + pFont.fileName + ".ttf";
	FontInstructions& font_instructions = pFont.instructions;

	FT_New_Face(ftLib, (fontPath).c_str(), 0, &ftface);
	

	pFont.instructions.pointSize = font_instructions.pointSize;

	setFontSize(ftface, pFont.fontMetrics, pFont.instructions);
	loadAtlas(ftface, font_instructions, pFont.fontMetrics, pFont.atlas, pFont.metrics);
	pFont.kerningMap = getKerningMap(ftface, font_instructions.startCode, font_instructions.glyphCount);
	

	FT_Done_Face(ftface);

}


void gl::GUI::Text::Initializer::
loadAtlas(FT_Face& pFace, FontInstructions& pFontInstructions, LoadFontMetrics& pFontMetrics, LoadAtlas& pAtlas, std::vector<LoadGlyphMetrics>& pMetrics)
{
	
	const unsigned int width_glyphs = (unsigned int)ceil(sqrt((float)pFontInstructions.glyphCount));
	unsigned int padPixels = 4;
	padPixels += padPixels % 2;
	unsigned int maxGlyphX = FT_MulFix(pFace->bbox.xMax - pFace->bbox.xMin, pFontMetrics.scale_x)/64;
	unsigned int maxGlyphY = FT_MulFix(pFace->bbox.yMax - pFace->bbox.yMin, pFontMetrics.scale_y)/64;
	
	unsigned int spread_pixels = 4;
	if (pFontInstructions.flags & FONT_LOAD_DT) {
		maxGlyphX = (unsigned int)((float)maxGlyphX / (float)pFontInstructions.upsampling) + spread_pixels * 2;
		maxGlyphY = (unsigned int)((float)maxGlyphY / (float)pFontInstructions.upsampling) +  spread_pixels * 2;
	}
	maxGlyphX += padPixels * 2;
	maxGlyphY += padPixels * 2;
	const unsigned int preWidth = (maxGlyphX * width_glyphs) + (maxGlyphX * width_glyphs) % 2;
	const unsigned int preHeight = (maxGlyphY * width_glyphs) + (maxGlyphY * width_glyphs) % 2;
	const unsigned int preAtlasTotalPixels = preWidth*preHeight;

	std::vector<unsigned char> buf(preAtlasTotalPixels);
	pAtlas.quads.resize(pFontInstructions.glyphCount);
	pMetrics.resize(pFontInstructions.glyphCount);
	unsigned int spread_pixels_up = spread_pixels * pFontInstructions.upsampling;
	unsigned int maxAtlasWidth = 0;
	unsigned int thisLineMaxY = 0;
	glm::uvec2 cursor = glm::uvec2();


	int maxAscend = 0;
	int maxDescend = 0;
	for (unsigned int g = 0; g < pFontInstructions.glyphCount; ++g) {
		//FreeType loads binary bitmap with 1 bit per pixel
		int flags = FT_LOAD_RENDER;
		if (FONT_LOAD_DT & pFontInstructions.flags) {

			flags = flags| FT_LOAD_TARGET_MONO;
		}
		FT_Load_Char(pFace, pFontInstructions.startCode + g, flags);
		FT_GlyphSlot& gly = pFace->glyph;

		//glyph usage data
		LoadGlyphQuad qud;


		LoadGlyphMetrics met;
		unsigned int glyWidth = gly->bitmap.width; //possibly upsampled size
		unsigned int glyHeight = gly->bitmap.rows;
		met.advanceX = gly->metrics.horiAdvance >> 6;
		met.xBearing = gly->bitmap_left; //(gly->metrics.horiBearingX >> 6);
		met.yBearing = gly->bitmap_top; //(gly->metrics.horiBearingY >> 6);
		
		
		//set atlas uv coords using the direct size of the current glyph, which may actually still need to be downsampled
		qud.minX = cursor.x + padPixels;
		qud.minY = cursor.y + padPixels;
		qud.maxX = qud.minX + glyWidth;
		qud.maxY = qud.minY + glyHeight;
		unsigned int bmpWidth = glyWidth + 2 * padPixels;
		unsigned int bmpHeight = glyHeight + 2 * padPixels;

		if (pFontInstructions.flags & FONT_LOAD_DT) {
			image<unsigned char>* inImage = nullptr;
			image<float>* innerOutImg = nullptr;
			image<float>* outerOutImg = nullptr;
			
			glyWidth += spread_pixels_up * 2;//new glyWidth, with upsampled spread padding; Its the size for the dt image
			glyHeight += spread_pixels_up * 2;
			qud.maxX = qud.minX + (unsigned int)ceil((float)glyWidth / (float)pFontInstructions.upsampling);
			qud.maxY = qud.minY + (unsigned int)ceil((float)glyHeight / (float)pFontInstructions.upsampling);
			bmpWidth = (qud.maxX - qud.minX)+ padPixels;
			bmpHeight = (qud.maxY - qud.minY) + padPixels;

			
			
			met.advanceX = (unsigned int)((float)met.advanceX / (float)pFontInstructions.upsampling);
			met.xBearing = (int)((float)met.xBearing / (float)pFontInstructions.upsampling) - spread_pixels;
			met.yBearing = (int)((float)met.yBearing / (float)pFontInstructions.upsampling) + spread_pixels;

			//std::vector<unsigned char> inImage(glyWidth * glyHeight);
			inImage = new image<unsigned char>(glyWidth, glyHeight);
			innerOutImg = new image<float>(glyWidth, glyHeight);
			outerOutImg = new  image<float>(glyWidth, glyHeight);
			unsigned int pitch = abs(gly->bitmap.pitch);
			//decode binary bitmap to full char bitmap if FT_LOAD_TARGET_MONO is used
			for (unsigned int line = 0; line < gly->bitmap.rows; ++line) {
				for (unsigned int byt = 0; byt < pitch; ++byt) {
					unsigned int bmpPos = (line*pitch) + byt;
					unsigned char chr = gly->bitmap.buffer[bmpPos];

					for (unsigned int bit = 0; bit < 8; ++bit) {
						unsigned char val = (chr >> (7 - bit)) & 1;
						unsigned int px = spread_pixels_up + (byt * 8) + bit;
						unsigned int py = line + spread_pixels_up;
						unsigned int pos = py * glyWidth + px;
						imRef(inImage, px, py) = val;
					}

				}
			}

			//for (unsigned int h = 0; h < glyHeight; ++h) {
			//	for (unsigned int w = 0; w < glyWidth; ++w) {
			//
			//		float val = inImage[w + h*glyWidth];
			//
			//		unsigned int bufpos = (cursor.y + padPixels + floor((float)h / (float)pFontInstructions.upsampling))*preWidth + cursor.x + padPixels + floor((float)w / (float)pFontInstructions.upsampling);
			//		buf[bufpos] += val *255.0f;
			//	}
			//
			//}

			//std::vector<float>outerOutImg = distanceTransform(glyWidth, glyHeight, inImage, 1);
			//std::vector<float>innerOutImg = distanceTransform(glyWidth, glyHeight, inImage, 0);
			innerOutImg = dt(inImage, 0);
			outerOutImg = dt(inImage, 1);
			float innerMaxDistance = 0.0f;
			//float outerMaxDistance = 0.0f;
			
			//find max distances
			for (unsigned int h = 0; h < glyHeight; ++h) {
				for (unsigned int w = 0; w < glyWidth; ++w) {
					innerMaxDistance = std::max(innerMaxDistance, imRef(innerOutImg, w, h));
					//outerMaxDistance = std::max(outerMaxDistance, outerOutImg[h*glyWidth + w]);
				}
			}
			
			
			//outerMaxDistance = sqrt(outerMaxDistance);
			innerMaxDistance = sqrt(innerMaxDistance);
			float max_dist = innerMaxDistance + innerMaxDistance;
			
			for (unsigned int h = 0; h < glyHeight; ++h) {
					for (unsigned int w = 0; w < glyWidth; ++w) {
			
						float outDist = sqrt(imRef(outerOutImg, w, h)); //values from 0 to outerMaxDistance
						float inDist = sqrt(imRef(innerOutImg, w, h)); //values from 0 to innerMaxDistance
						float val = (((innerMaxDistance - std::min(outDist, innerMaxDistance)) + inDist) / max_dist);
			
						val = val/(float)square(pFontInstructions.upsampling);
						unsigned int bufpos = (cursor.y + padPixels + (unsigned int)round((float)h / (float)pFontInstructions.upsampling))*preWidth + cursor.x + padPixels + (unsigned int)round((float)w / (float)pFontInstructions.upsampling);
						buf[bufpos] += (unsigned char)(val *255.0f);
					}
				
			}
			delete inImage;
			delete innerOutImg;
			delete outerOutImg;
			//endif FONT_LOAD_DT
		}
		else {//regular load
			
			
			for (unsigned int line = 0; line < gly->bitmap.rows; ++line) {
				unsigned int bmpPos = (line*gly->bitmap.width);
				unsigned int pos = ((line + qud.minY)*preWidth) + qud.minX;
				std::memcpy(&buf[pos], &gly->bitmap.buffer[bmpPos], sizeof(unsigned char)*gly->bitmap.width);
			}
		}
		

		//advance atlas cursor
		cursor.x += (bmpWidth);
		thisLineMaxY = std::max(bmpHeight, thisLineMaxY);
		maxAscend = std::max(maxAscend, met.yBearing);
		maxDescend = std::max(maxDescend, (int)bmpHeight - (met.yBearing + (int)padPixels*2));
		pAtlas.quads[g] = qud;
		pMetrics[g] = met;
				
		//check whether to start a new line of glyphs in the atlas
		if ((cursor.x + (maxGlyphX)) > preWidth || g + 1 == pFontInstructions.glyphCount ) {
			maxAtlasWidth = std::max(maxAtlasWidth, cursor.x);
			cursor.x = 0;
			cursor.y += thisLineMaxY;

		}
		
	}

	pAtlas.width = maxAtlasWidth;
	pAtlas.height = cursor.y;

	const unsigned int totalPixelCount = pAtlas.width * pAtlas.height;
	pAtlas.buffer.resize(totalPixelCount);
	pFontMetrics.lineGap = maxAscend + maxDescend;
	for (unsigned int h = 0; h < pAtlas.height; ++h) {
		std::memcpy(&pAtlas.buffer[h*pAtlas.width], &buf[h*preWidth], sizeof(unsigned char)*pAtlas.width);
	}
}


void gl::GUI::Text::Initializer::
initFreeType()
{
	if (FT_Init_FreeType(&ftLib)) {
		App::Debug::pushError("Failed to init FreeType!");
	}
	
	gl::Debug::getGLError("initFOntLoader():");
}

unsigned int gl::GUI::Text::Initializer::
includeFont(std::string pFontFileName, unsigned int pPointSize,  unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling)
{
	return includeFont(pFontFileName, FontInstructions(pPointSize, pStartCode, pGlyphCount, pFlags, pUpsampling));
}

unsigned int gl::GUI::Text::Initializer::
includeFont(std::string pFontFileName, FontInstructions& pLoadInstructions)
{
	unsigned int sz = loadFontBuffer.size();
	LoadFont font;
	removeFileExtension(pFontFileName);
	font.fileName = pFontFileName;
	font.instructions = pLoadInstructions;
	loadFontBuffer.push_back(font);
	return sz;
}
unsigned int gl::GUI::Text::Initializer::
createFontInstructions(unsigned int pPointSize, unsigned int pStartCode, unsigned int pGlyphCount, int pFlags, unsigned int pUpsampling)
{
	return createFontInstructions(FontInstructions(pPointSize, pStartCode, pGlyphCount, pFlags, pUpsampling));
}

unsigned int gl::GUI::Text::Initializer::
createFontInstructions(FontInstructions & pInstructions)
{
	unsigned int sz = allFontInstructions.size();
	unsigned int i = std::find(allFontInstructions.begin(), allFontInstructions.end(), pInstructions) - allFontInstructions.begin();
	if (i < sz) {
		return i;
	}
	allFontInstructions.push_back(pInstructions);
	return sz;
}

void gl::GUI::Text::Initializer::
removeFileExtension(std::string& pFileName) {
	unsigned int nameCharCount = pFileName.size();
	for (unsigned int i = 0; i < 4; ++i) {
		if (pFileName[nameCharCount - i - 1] == '.') {
			nameCharCount = nameCharCount - i - 1;
			break;
		}
	}
	pFileName.resize(nameCharCount);
}

//Font file integration
gl::GUI::Text::Font gl::GUI::Text::Initializer::
integrateFont(LoadFont& pFont) {
	Debug::getGLError("before integrateFont():");
	App::Debug::printErrors();
	Font font;//TODO make font support multiple fontSizes
	glGenTextures(1, &font.atlasID);
	glBindTexture(GL_TEXTURE_2D, font.atlasID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	

	storeGlyphs(font, pFont);
	std::pair<unsigned int, unsigned int> range = convertKerning(pFont.kerningMap);
	font.kerningOffset = range.first;
	font.kerningCount = range.second;
	font.fontMetric.lineGap = (float)pFont.fontMetrics.lineGap / ((float)screenPixelHeight / 2.0f);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, pFont.atlas.width, pFont.atlas.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &pFont.atlas.buffer[0]);
		
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	unsigned int off = allFontInstructions.size();
	allFontInstructions.push_back(pFont.instructions);
	font.instructions = off;
	return font;
}

std::pair<unsigned int, unsigned int> gl::GUI::Text::Initializer::
convertKerning(std::vector<int>& pKerningMap)
{
	unsigned int count = pKerningMap.size();
	unsigned int offset = allKerning.size();
	allKerning.resize(offset + count);
	for (unsigned int i = 0; i < count; ++i) {
		allKerning[offset + i] = (float)pKerningMap[i] / ((float)screenPixelWidth / 2.0f);
	}
	return std::pair<unsigned int, unsigned int>(offset,count);
}

void gl::GUI::Text::Initializer::
storeGlyphs(Font& pFont, const LoadFont & pLoadFont)
{
	unsigned int glyCount = pLoadFont.atlas.quads.size();
	std::vector<Glyph> glyphs(glyCount);
	pFont.metricOffset = allMetrics.size();
	pFont.metricCount = glyCount;
	allMetrics.resize(pFont.metricOffset + pFont.metricCount);

	for (unsigned int g = 0; g < glyCount; ++g) {
		const LoadGlyphQuad& qud = pLoadFont.atlas.quads[g];
		const LoadGlyphMetrics& met = pLoadFont.metrics[g];
		glyphs[g] = Glyph((float)qud.minX / (float)pLoadFont.atlas.width, (float)qud.minY / (float)pLoadFont.atlas.height, (float)(qud.maxX) / (float)pLoadFont.atlas.width, (float)(qud.maxY) / (float)pLoadFont.atlas.height);
		allMetrics[pFont.metricOffset + g] = GlyphMetrics((float)(qud.maxX - qud.minX) / ((float)screenPixelWidth / 2.0f), (float)(qud.maxY - qud.minY) / ((float)screenPixelHeight / 2.0f), (float)met.advanceX / ((float)screenPixelWidth / 2.0f), (float)met.xBearing / ((float)screenPixelWidth / 2.0f), (float)met.yBearing / ((float)screenPixelHeight / 2.0f));
		//allMetrics[pSize.metricOffset + g] = GlyphMetrics((float)met.width, (float)met.height, (float)met.advanceX, (float)met.xBearing, (float)met.yBearing);
	}
	pFont.glyphStorageIndex = Shader::Data::createStorage(sizeof(Glyph)*glyCount, &glyphs[0], 0);
	Shader::Data::bindStorage(GL_UNIFORM_BUFFER, pFont.glyphStorageIndex);

}

//Font Serialization
void gl::GUI::Text::Initializer::
setFontInputDir(std::string pNewDirectory){
	
	font_input_directory = pNewDirectory;
}

void gl::GUI::Text::Initializer::
setFontStoreDir(std::string pNewDirectory) {

	font_store_directory = pNewDirectory;
}

void gl::GUI::Text::Initializer::
saveFontFile(std::string pFileName, LoadFont * pFont)
{
	puts("saving font file...");
	std::string filePath(font_store_directory + pFileName);
	removeFileExtension(pFileName);
	FontInstructions& instructions = pFont->instructions;
	std::string instruction_str('_' + std::to_string(instructions.pointSize) +  '_' + std::to_string(instructions.startCode) + '_' + std::to_string(instructions.glyphCount));
	filePath += instruction_str + ".gaf";

	std::ofstream file(filePath);
	{
		
	}

}

int gl::GUI::Text::Initializer::
loadFontFile(std::string pFileName, LoadFont* pFont)
{
	puts("loading font file...");
	int ret = 0;
	std::string filePath(font_store_directory + pFileName);
	FontInstructions& instructions = pFont->instructions;
	std::string instruction_str('_' + std::to_string(instructions.pointSize) + '_' + std::to_string(instructions.startCode) + '_' + std::to_string(instructions.glyphCount));
	filePath += instruction_str + ".gaf";
	std::ifstream ifs(filePath);
	if (ifs.fail()) {
		ret = 1;
		puts("fail!");
	}
	else {
		
	}

	return ret;
}