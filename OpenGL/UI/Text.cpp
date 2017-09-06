#include "..\Global\stdafx.h"
#include "Text.h"
#include "..\BaseGL\Shader.h"
#include "..\Global\glDebug.h"
#include <algorithm>
#include "..\Camera.h"
#include <App\Global\Debug.h>
#include <glm\gtc\matrix_transform.hpp>


std::vector<gl::GUI::Text::Textbox> gl::GUI::Text::allTextboxes;
std::vector<gl::GUI::Text::TextMetrics> gl::GUI::Text::allTextMetrics;
unsigned int gl::GUI::Text::glyphShapeProgram = 0;
unsigned int gl::GUI::Text::fontCount = 0;
unsigned int gl::GUI::Text::MAX_CHARS = 1000;
std::vector<gl::GUI::Text::CharQuad> gl::GUI::Text::charQuadBuffer;
std::vector<unsigned int> gl::GUI::Text::glyphIndexBuffer;
std::vector<unsigned char> gl::GUI::Text::charBuffer;
std::vector<gl::GUI::Text::Font> gl::GUI::Text::allFonts;
unsigned int gl::GUI::Text::quadStorage = 0;
unsigned int gl::GUI::Text::charStorage = 0;
std::vector<gl::GUI::Text::LoadString> gl::GUI::Text::loadStringBuffer;
std::vector<gl::GUI::Text::String> gl::GUI::Text::stringBuffer;
std::vector<float> gl::GUI::Text::allKerning;
std::vector<gl::GUI::Text::GlyphMetrics> gl::GUI::Text::allMetrics;
unsigned int gl::GUI::Text::fontVAO = 0;
std::vector<gl::GUI::Text::FontInstructions> gl::GUI::Text::allFontInstructions;
std::vector<gl::GUI::Text::StringRenderInstructions> gl::GUI::Text::allStrRenderInstructions;


/*USAGE
- create a texbox by specifying a position and size etc
- 
- calculate the string layout of each textbox
- transform each character and add them to the rendering buffer

*/
gl::GUI::Text::Textbox* gl::GUI::Text::
createTextbox(glm::vec2* pTopLeft, glm::vec2* pSize, TextMetrics* pMetrics, int pFlags, float pMarging) {
	Textbox tb;
	
	tb.pos = pTopLeft;
	tb.size = pSize;
	tb.metrics = pMetrics;
	tb.marging = pMarging;
	tb.flags = pFlags;
	allTextboxes.push_back(tb);
	return &allTextboxes.back();
}

gl::GUI::Text::Textbox *
gl::GUI::Text::createTextbox(Quad pQuad, TextMetrics * pMetrics, int pFlags, float pMarging)
{
	return createTextbox(&gl::GUI::allPositions[gl::GUI::allQuads[pQuad].pos], &gl::GUI::allSizes[gl::GUI::allQuads[pQuad].size], pMetrics, pFlags, pMarging);
}

void gl::GUI::Text::
insertTextboxString(Textbox* pTextbox, LoadString pString)
{
	pString.textbox = pTextbox;
	if (!pTextbox->stringCount++) {
		pTextbox->stringOffset = loadStringBuffer.size();
		loadStringBuffer.push_back(pString);
		return;
	}
	loadStringBuffer.insert(loadStringBuffer.begin() + pTextbox->stringOffset, pString);
}

gl::GUI::Text::LoadString gl::GUI::Text::createString(std::string pChars, StringRenderInstructions * pInstructions, unsigned int pFontIndex)
{
	LoadString fs = LoadString(String(charBuffer.size(), pChars.size(), pInstructions), pFontIndex);
	charBuffer.insert(charBuffer.end(), pChars.begin(), pChars.end());
	return fs;
}




gl::GUI::Text::TextMetrics* gl::GUI::Text::createTextMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
{
	allTextMetrics.push_back(TextMetrics(glm::vec2(pGlyphScaleX, pGlyphScaleY), pAdvanceScale, pLineGapScale));
	return &allTextMetrics.back();
}

gl::GUI::Text::String gl::GUI::Text::
loadString(LoadString& pString)
{
	Font& font = allFonts[pString.fontIndex];
	FontInstructions& inst = allFontInstructions[font.instructions];
	
	Textbox& tb = *pString.textbox;
	TextMetrics& textMetrics = *tb.metrics;
	StringRenderInstructions& r_inst = *pString.string.render_instructions;

	
	std::vector<unsigned int> charIndices(pString.string.charCount);//for glyph textures

	//calculate string size
	unsigned int charCount = pString.string.charCount;
	glm::vec2 cursor = glm::vec2(0.0f, 0.0f);
	unsigned int leftIndex = 0;
	unsigned int rightIndex = 0;
	unsigned int nonCharCount = 0;
	float string_width = 0.0f;
	float string_height = 0.0f;
	for (unsigned int c = 0; c < charCount; ++c) {
		//prevent right to be less than 0 or more than (glyphCount - 1)
		unsigned char charCode = charBuffer[pString.string.charOffset + c];
		rightIndex = std::max((unsigned int)0, std::min(charCode - inst.startCode, inst.glyphCount - 1));
		GlyphMetrics& met = allMetrics[font.metricOffset + rightIndex];
		if (charCode == '\n' || (cursor.x + met.advanceX * textMetrics.advanceScale)*!(tb.flags & TEXT_LAYOUT_FORCE_X_FIT) > (tb.size->x - tb.marging*2.0f)) {
			if (charCode == '\n') {
				++nonCharCount;
				leftIndex = 0;
			}
			string_width = std::max(string_width, cursor.x);
			string_height += font.fontMetric.lineGap*textMetrics.lineGapScale;
			cursor = glm::vec2(0.0f, cursor.y - font.fontMetric.lineGap*textMetrics.lineGapScale);
			continue;
		}
		
		charIndices[c - nonCharCount] = rightIndex;
		
		leftIndex = rightIndex;
		cursor.x += met.advanceX * textMetrics.advanceScale;
		string_width = std::max(string_width, cursor.x);
		string_height = std::max(string_height, met.bearingY*textMetrics.glyphScale.y);
	}
	charCount -= nonCharCount;
	std::vector<CharQuad> quads(charCount);//for positioning
	charIndices.resize(charCount);

	//transform glyphs
	cursor = *tb.pos;
	float fit_x = 1.0f;
	float fit_y = 1.0f;
	if (!(tb.flags ^ TEXT_LAYOUT_FORCE_X_FIT)) {
		fit_x = std::min(1.0f, (tb.size->x + tb.marging*2.0f) / string_width);
	}
	if (!(tb.flags ^ TEXT_LAYOUT_FORCE_Y_FIT)) {
		fit_y = std::min(1.0f, tb.size->y / string_height);
	}

	if (!(tb.flags ^ TEXT_LAYOUT_CENTER)) {
		cursor = glm::vec2(cursor.x + tb.marging + (tb.size->x / 2.0f) - string_width / 2.0f , cursor.y - (tb.size->y / 2.0f) - string_height/2.5f);
	}
	else if (!(tb.flags ^ TEXT_LAYOUT_BOUND_LEFT)) {
		cursor = glm::vec2(cursor.x + tb.marging, cursor.y - (tb.size->y / 2.0f) - string_height / 2.5f);
	}
	else if (!(tb.flags ^ TEXT_LAYOUT_BOUND_RIGHT)) {
		cursor = glm::vec2((cursor.x + tb.size->x) - string_width + tb.marging, cursor.y - (tb.size->y / 2.0f) - string_height / 2.5f);
	}
	for (unsigned int g = 0; g < charCount; ++g) {
		unsigned char charindex = charIndices[g];
		GlyphMetrics& met = allMetrics[charindex];//may be unsafe(?) look here for bugs ... ;D
		
		//float kern = allKerning[font.kerningOffset + leftIndex*inst.glyphCount + rightIndex] * textMetrics.advanceScale;
		quads[g] = CharQuad(cursor.x + met.bearingX*textMetrics.advanceScale*fit_x, cursor.y + met.bearingY*textMetrics.glyphScale.y*fit_y, met.width*textMetrics.glyphScale.x*fit_x, met.height*textMetrics.glyphScale.y*fit_y);
		cursor.x += met.advanceX * textMetrics.advanceScale * fit_x;
		
	}

	

	pString.string.charOffset = charQuadBuffer.size();
	pString.string.charCount -= nonCharCount;
	
	charQuadBuffer.resize(pString.string.charOffset + pString.string.charCount);
	glyphIndexBuffer.resize(pString.string.charOffset + pString.string.charCount);

	std::memcpy(&charQuadBuffer[pString.string.charOffset], &quads[0], sizeof(CharQuad)*pString.string.charCount);
	std::memcpy(&glyphIndexBuffer[pString.string.charOffset], &charIndices[0], sizeof(unsigned int)*pString.string.charCount);

	return pString.string;
}

void gl::GUI::Text::
loadStrings()
{
	stringBuffer.reserve(loadStringBuffer.size());
	for (unsigned int s = 0; s < loadStringBuffer.size(); ++s) {
		
		LoadString& str = loadStringBuffer[s];
		Font& font = allFonts[str.fontIndex];
		String ostr = loadString(str);
		if (!font.stringCount++) {
			font.stringOffset = stringBuffer.size();
			stringBuffer.push_back(ostr);
			continue;
		}
		stringBuffer.insert(stringBuffer.begin() + font.stringOffset, ostr);
	}
	loadStringBuffer.clear();
}



gl::GUI::Text::StringRenderInstructions* gl::GUI::Text::createStringRenderInstructions(StringRenderInstructions & pInstructions)
{
	allStrRenderInstructions.push_back(pInstructions);
	return &allStrRenderInstructions.back();
}

gl::GUI::Text::StringRenderInstructions * gl::GUI::Text::createStringRenderInstructions(float pThickness, float pHardness, float pDepth, glm::vec4 pColor)
{
	return createStringRenderInstructions(StringRenderInstructions(pThickness, pHardness, pDepth, pColor));
}

void gl::GUI::Text::
renderGlyphs()
{		

	glBindVertexArray(fontVAO);
	Shader::use(glyphShapeProgram);

	glActiveTexture(GL_TEXTURE0);

	glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	static float ref = 0.5f;
	Shader::setUniform(glyphShapeProgram, "atlas", 0);
	//VAO::setUniform(glyphShapeProgram, "ortho", ortho);
	for (unsigned int fo = 0; fo < allFonts.size(); ++fo) {
		Font& font = allFonts[fo];
		glBindTexture(GL_TEXTURE_2D, font.atlasID);
		Shader::bindBufferToShader(glyphShapeProgram, font.glyphStorageIndex, "GlyphBuffer");
		for (unsigned int s = 0; s < font.stringCount; ++s) {
			String& str = stringBuffer[font.stringOffset + s];
			StringRenderInstructions& inst = *str.render_instructions;
			Shader::setUniform(glyphShapeProgram, "hardness", inst.hardness);
			Shader::setUniform(glyphShapeProgram, "thickness", inst.thickness);
			Shader::setUniform(glyphShapeProgram, "color", inst.color);
			Shader::setUniform(glyphShapeProgram, "depth", inst.depth);



			glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, str.charCount, str.charOffset);

			gl::Debug::getGLError("renderAtlas():");
			
		}		
	}
	
	Shader::unuse();
	glBindVertexArray(0);
}



void gl::GUI::Text::
updateCharStorage()
{
	CharQuad* charQuadPtr = nullptr;
	unsigned int* glyphIndexPtr = nullptr;
	if (charQuadBuffer.size()) {
		charQuadPtr = &charQuadBuffer[0];
		glyphIndexPtr = &glyphIndexBuffer[0];
	
	VAO::streamStorage(quadStorage, sizeof(CharQuad)*charQuadBuffer.size(), charQuadPtr);
	VAO::streamStorage(charStorage, sizeof(unsigned int)*glyphIndexBuffer.size(), glyphIndexPtr);
	}
}

void gl::GUI::Text::
clearCharStorage()
{
	for (Font& font : allFonts) {
		font.stringOffset = 0;
		font.stringCount = 0;
	}
	allTextboxes.clear();
	charBuffer.clear();
	glyphIndexBuffer.clear();
	charQuadBuffer.clear();
	stringBuffer.clear();
	
}

void gl::GUI::Text::
initFontShader()
{
	glyphShapeProgram = Shader::newProgram("glyphShapeShader", Shader::newModule("glyphShapeShader.vert"), Shader::newModule("glyphShapeShader.frag")).ID;
	Shader::addVertexAttribute(glyphShapeProgram, "pos", 0);
	Shader::addVertexAttribute(glyphShapeProgram, "quad", 1);
	Shader::addVertexAttribute(glyphShapeProgram, "index", 2);
}

void gl::GUI::Text::
initFontVAO() {
	glCreateVertexArrays(1, &fontVAO);
	//Quad triangles 
	glVertexArrayVertexBuffer(fontVAO, 0, quadVBO, 0, sizeof(float) * 2);
	VAO::initVertexAttrib(fontVAO, 0, 0, 2, GL_FLOAT, 0);
	//quad position and size
	quadStorage = VAO::createStorage(MAX_CHARS * sizeof(CharQuad) * 3, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(quadStorage, GL_MAP_WRITE_BIT);
	VAO::bindVertexArrayVertexStorage(fontVAO, 1, quadStorage, sizeof(float) * 4);
	VAO::initVertexAttrib(fontVAO, 1, 1, 4, GL_FLOAT, 0);
	
	//glyph index
	charStorage = VAO::createStorage(MAX_CHARS * sizeof(unsigned int) * 3, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(charStorage, GL_MAP_WRITE_BIT);
	VAO::bindVertexArrayVertexStorage(fontVAO, 2, charStorage, sizeof(unsigned int));
	VAO::initVertexAttrib(fontVAO, 2, 2, 1, GL_UNSIGNED_INT, 0);

	glVertexArrayBindingDivisor(fontVAO, 1, 1);
	glVertexArrayBindingDivisor(fontVAO, 2, 1);

	glVertexArrayElementBuffer(fontVAO, quadEBO);
}


