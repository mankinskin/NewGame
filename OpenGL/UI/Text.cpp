#include "..\Global\stdafx.h"
#include "Text.h"
#include "..\BaseGL\Shader.h"
#include "..\Global\glDebug.h"
#include <algorithm>
#include "..\Camera.h"
#include <App\Global\Debug.h>
#include <glm\gtc\matrix_transform.hpp>


std::vector<gl::GUI::Text::Textbox> gl::GUI::Text::allTextboxes;
std::vector<gl::GUI::Text::TextboxMetrics> gl::GUI::Text::allTextboxMetrics;
std::vector<gl::GUI::Text::TextStyle> gl::GUI::Text::allTextStyles;
std::vector<gl::GUI::Text::String> gl::GUI::Text::allStrings;
std::vector<unsigned char> gl::GUI::Text::allChars;
std::vector<unsigned int> gl::GUI::Text::textboxStringIndices;
std::vector<gl::GUI::Text::String> gl::GUI::Text::allFontStrings;
std::vector<gl::GUI::Text::CharQuad> gl::GUI::Text::charQuadBuffer;
std::vector<unsigned int> gl::GUI::Text::glyphIndexBuffer;
unsigned int gl::GUI::Text::styleStorage = 0;
std::vector<glm::vec2> gl::GUI::Text::allTextboxPositions;
std::vector<glm::vec2> gl::GUI::Text::allTextboxSizes;
std::vector<glm::vec4> gl::GUI::Text::allTextColors;
//std::vector<std::string> gl::GUI::Text::allTextColorNames;
/*USAGE
- create textbox: set pos, size, render_instructions and font
- set textbox string: set chars to write on screen
- parse textbox strings
   - extract render instructions
   - split strings 
   - create RenderStates

*/

void gl::GUI::Text::reserveTextboxSpace(unsigned int pCount) {

	gl::GUI::Text::allTextboxes.reserve(pCount);
	gl::GUI::Text::allTextboxPositions.reserve(pCount);
	gl::GUI::Text::allTextboxSizes.reserve(pCount);
}

unsigned int gl::GUI::Text::createTextbox(unsigned int pQuadIndex, unsigned int pMetrics, int pFlags, float pMarging) {
	RefQuad qd = allQuads[pQuadIndex];
	return createTextbox(allPositions[qd.pos], allSizes[qd.size], pMetrics, pFlags, pMarging);
}

unsigned int gl::GUI::Text::
createTextbox(unsigned int pPosIndex, unsigned int pSizeIndex, unsigned int pMetrics, int pFlags, float pMarging) {
	Textbox tb;
	tb.pos = pPosIndex;
	tb.size = pSizeIndex;
	tb.metrics = pMetrics;
	tb.marging = pMarging;
	tb.flags = pFlags;
	allTextboxes.push_back(tb);
	return allTextboxes.size() - 1;
}

unsigned int
gl::GUI::Text::createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging)
{
	allTextboxPositions.push_back(pTopLeft);
	allTextboxSizes.push_back(pSize);
	return createTextbox(allTextboxPositions.size() - 1, allTextboxSizes.size() - 1, pMetrics, pFlags, pMarging);
}
void gl::GUI::Text::
setTextboxString(unsigned int pTextbox, std::string pString)
{
	setTextboxString(pTextbox, String(pString));
}

void gl::GUI::Text::
setTextboxString(unsigned int pTextbox, String pString)
{
	Textbox& tb = allTextboxes[pTextbox];
	tb.chars = pString;
}

void gl::GUI::Text::revalidateTextboxCharIndices()
{
	unsigned int off = 0;
	for (Textbox& tb : allTextboxes) {
		tb.chars.offset = off;
		off += tb.chars.count;
	}
}

void gl::GUI::Text::revalidateFontStringIndices()
{
	unsigned int off = 0;
	for (Font& fon : allFonts) {
		fon.stringOffset = off;
		off += fon.stringCount;
	}
}

void gl::GUI::Text::insertFontString(Font & pFont, String pString)
{
	if (!pFont.stringCount) {//if first textbox of this font
		pFont.stringOffset = allFontStrings.size();//dedicate a new range of tb indices to this font
	}
	++pFont.stringCount;
	allFontStrings.insert(allFontStrings.begin() + pFont.stringOffset, pString);
}


unsigned int gl::GUI::Text::createTextboxMetrics(unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
{
	allTextboxMetrics.push_back(TextboxMetrics(pFont, glm::vec2(pGlyphScaleX, pGlyphScaleY), pAdvanceScale, pLineGapScale));
	return allTextboxMetrics.size() - 1;
}

unsigned int gl::GUI::Text::createTextStyle(TextStyle & pStyle)
{
	allTextStyles.push_back(pStyle);
	return allTextStyles.size() - 1;
}

unsigned int gl::GUI::Text::createTextStyle(float pThickness, float pHardness)
{
	return createTextStyle(TextStyle(pThickness, pHardness));
}

unsigned int gl::GUI::Text::createTextColor(glm::vec4 pColor)
{
	allTextColors.push_back(pColor);
	return allTextColors.size() - 1;
}

void gl::GUI::Text::initStyleBuffer() {

	allTextStyles.reserve(2);
	createTextStyle(1.5f, 0.8f);
	createTextStyle(1.2f, 0.8f);
	styleStorage = VAO::createStorage(sizeof(TextStyle)*allTextStyles.size(), &allTextStyles[0], 0);
	
	Shader::bindUniformBufferToShader(glyphShapeProgram, styleStorage, "StyleBuffer");
}

void gl::GUI::Text::
loadTextboxes()
{
	for (unsigned int t = 0; t < allTextboxes.size(); ++t) {
		Textbox& tb = allTextboxes[t];
		TextboxMetrics& textMetrics = allTextboxMetrics[tb.metrics];
		Font& font = allFonts[textMetrics.font];
		
		TextboxGlyphs tbGlyphs(tb.chars.count);
		//load word strings and get style and color indices
		//load lines and their sizes
		//transform all glyphs with formatting
		parseStringFormat(tb.chars);
		loadTextboxGlyphs(tb, textMetrics, font, tbGlyphs);
		transformTextboxGlyphs(tb, tbGlyphs);

		unsigned int buffer_offset = charQuadBuffer.size();
		insertFontString(font, String(buffer_offset, tbGlyphs.quads.size())); //insert the string of this textbox for this font
		
		charQuadBuffer.resize(buffer_offset + tbGlyphs.quads.size());
		std::memcpy(&charQuadBuffer[buffer_offset], &tbGlyphs.quads[0], sizeof(CharQuad)*tbGlyphs.quads.size());
		
		glyphIndexBuffer.resize(buffer_offset + tbGlyphs.glyphIndices.size());
		std::memcpy(&glyphIndexBuffer[buffer_offset], &tbGlyphs.glyphIndices[0], sizeof(unsigned int)*tbGlyphs.glyphIndices.size());
		
	}
	revalidateFontStringIndices();
}

void gl::GUI::Text::parseStringFormat(String pString)
{
	
}

void gl::GUI::Text::
loadTextboxGlyphs(Textbox& pTextbox, TextboxMetrics& pTextMetrics, Font& pFont, TextboxGlyphs& pGlyphs)
{
	float cursor = 0.0f;
	FontInstructions& font_inst = allFontInstructions[pFont.instructions];
	
	glm::vec2 thisLineSize = glm::vec2();
	String thisLineStr = String(0, 0);
	unsigned int str_char_off = 0; //the offset of the current string (in pGlyphs.glyphIndices & pGlyphs.quads)
	unsigned int line_char_off = 0;//the offset of the current line
	unsigned int lineCharCount = 0;
	float thisLineGreatestAscend = 0.0f;
	float thisLineGreatestDescend = 0.0f;
	
	unsigned int glyphIndex = 0;
	unsigned int str_nonCharCount = 0;
	//iterate string chars
	for (unsigned int c = 0; c < pTextbox.chars.count;) {
		//get glyph metrics
		unsigned int charCode = allChars[pTextbox.chars.offset + c];
		glyphIndex = std::max((unsigned int)0, std::min(charCode - font_inst.startCode, font_inst.glyphCount - 1));
		GlyphMetrics& met = allMetrics[pFont.metricOffset + glyphIndex];
		
		if (charCode != '\n' && (cursor + met.advanceX * pTextMetrics.advanceScale) < (allTextboxSizes[pTextbox.size].x - pTextbox.marging*2.0f) ) {
			//append char to all chars
			CharQuad qd(cursor+ met.bearingX, met.bearingY, met.width * pTextMetrics.glyphScale.x, met.height * pTextMetrics.glyphScale.y);
			thisLineGreatestAscend = std::max(thisLineGreatestAscend, met.bearingY);
			thisLineGreatestDescend = std::max(thisLineGreatestDescend, met.height - met.bearingY);
			pGlyphs.quads[str_char_off + c - str_nonCharCount] = qd;
			pGlyphs.glyphIndices[str_char_off + c - str_nonCharCount] = glyphIndex;

			cursor += met.advanceX * pTextMetrics.advanceScale;
			++lineCharCount;
			++c;
		}
		else {
			//end line and start new line
			if (charCode == '\n') {
				++str_nonCharCount;
				++c;
			}
			
			thisLineSize.y = std::max(thisLineSize.y, thisLineGreatestAscend + thisLineGreatestDescend);
			thisLineSize.x = cursor;
			pGlyphs.line_sizes.push_back(thisLineSize);

			thisLineStr = String(line_char_off, lineCharCount);
			pGlyphs.lines.push_back(thisLineStr);

			thisLineGreatestAscend = 0.0f;
			thisLineGreatestDescend = 0.0f;
			cursor = 0.0f;
			line_char_off += lineCharCount;
			lineCharCount = 0;
		}
	}//end loop over string chars
	str_char_off += pTextbox.chars.count - str_nonCharCount;
	thisLineSize.y = std::max(thisLineSize.y, thisLineGreatestAscend + thisLineGreatestDescend);
	thisLineSize.x = cursor;
	pGlyphs.line_sizes.push_back(thisLineSize);

	thisLineStr = String(line_char_off, lineCharCount);
	pGlyphs.lines.push_back(thisLineStr);
	pGlyphs.line_sizes.shrink_to_fit();
	pGlyphs.lines.shrink_to_fit();
	pGlyphs.glyphIndices.resize(str_char_off);
	pGlyphs.quads.resize(str_char_off);
}



void gl::GUI::Text::transformTextboxGlyphs(Textbox& pTextbox, TextboxGlyphs& pGlyphs)
{
	glm::vec2 tb_pos = allTextboxPositions[pTextbox.pos];
	glm::vec2 tb_size = allTextboxSizes[pTextbox.size];
	TextboxMetrics& textMetrics = allTextboxMetrics[pTextbox.metrics];
	Font& font = allFonts[textMetrics.font];
	
	float allLineHeight = font.fontMetric.lineGap * pGlyphs.lines.size();
	glm::vec2 cursor = glm::vec2(tb_pos.x, tb_pos.y);
	if (pTextbox.flags & TEXT_LAYOUT_FREE_LINES) {
		allLineHeight = 0.0f;
		for (unsigned int l = 0; l < pGlyphs.lines.size(); ++l) {
			allLineHeight += pGlyphs.line_sizes[l].y * textMetrics.lineGapScale;
		}
	}
	if (pTextbox.flags & TEXT_LAYOUT_CENTER_Y && !(pTextbox.flags & TEXT_LAYOUT_FREE_LINES)) {
		cursor.y = (cursor.y - (tb_size.y / 2.0f)) + allLineHeight/2.0f;
	}
	for (unsigned int l = 0; l < pGlyphs.lines.size(); ++l) {
		String& line = pGlyphs.lines[l];
		glm::vec2& line_size = pGlyphs.line_sizes[l];
		if (pTextbox.flags & TEXT_LAYOUT_FREE_LINES) {
			cursor.y -= pGlyphs.line_sizes[l].y * textMetrics.lineGapScale;
		}
		else {
			cursor.y -= font.fontMetric.lineGap;
		}
		if (pTextbox.flags & TEXT_LAYOUT_CENTER_X) {
			cursor.x = tb_pos.x + (tb_size.x / 2.0f) - line_size.x / 2.0f;
		}
		else if (pTextbox.flags & TEXT_LAYOUT_BOUND_LEFT) {
			cursor.x = tb_pos.x + pTextbox.marging;
		}
		else if (pTextbox.flags & TEXT_LAYOUT_BOUND_RIGHT) {
			cursor.x = (tb_pos.x + tb_size.x) - (line_size.x + pTextbox.marging);
		}
		for (unsigned int g = 0; g < line.count; ++g) {
			CharQuad& qd = pGlyphs.quads[line.offset + g];
			qd.pos += cursor;
		}
		
	}

}

void gl::GUI::Text::
renderGlyphs()
{
	if (allTextboxes.size()) {
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(fontVAO);
		Shader::use(glyphShapeProgram);
		glActiveTexture(GL_TEXTURE0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Shader::setUniform(GUI::Text::glyphShapeProgram, "atlas", (int)0);
		//VAO::setUniform(glyphShapeProgram, "ortho", ortho);
		
		for (unsigned int fo = 0; fo < allFonts.size(); ++fo) {
			Font& font = allFonts[fo];
			glBindTexture(GL_TEXTURE_2D, font.atlasID);
			Shader::bindUniformBufferToShader(glyphShapeProgram, font.glyphStorageIndex, "GlyphBuffer");
			
			for (unsigned int s = 0; s < font.stringCount; ++s) {
				String& str = allFontStrings[font.stringOffset + s];
				Shader::setUniform(glyphShapeProgram, "styleIndex", (unsigned int)0);

				glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, str.count, str.offset);
				
			}
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Debug::getGLError("renderGlyphs():");
		Shader::unuse();
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}

gl::GUI::Text::String::String(std::string pString)
:offset(allChars.size()), count(pString.size()) {
	allChars.insert(allChars.end(), pString.begin(), pString.end());
}

void gl::GUI::Text::
updateCharStorage()
{
	if (charQuadBuffer.size()) {
		VAO::streamStorage(quadStorage, sizeof(CharQuad)*charQuadBuffer.size(), &charQuadBuffer[0]);
		VAO::streamStorage(charStorage, sizeof(unsigned int)*glyphIndexBuffer.size(), &glyphIndexBuffer[0]);
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
	allChars.clear();
	glyphIndexBuffer.clear();
	charQuadBuffer.clear();
	allStrings.clear();
	allFontStrings.clear();
	textboxStringIndices.clear();
}
