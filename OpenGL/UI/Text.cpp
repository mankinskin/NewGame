#include "..\Global\stdafx.h"
#include "Text.h"
#include "..\BaseGL\Shader.h"
#include "..\Global\glDebug.h"
#include <algorithm>
#include "..\Camera.h"
#include <App\Global\Debug.h>
#include <glm\gtc\matrix_transform.hpp>
#include "Font.h"

std::vector<gl::GUI::Text::Textbox> gl::GUI::Text::allTextboxes;
std::vector<gl::GUI::Text::TextboxMetrics> gl::GUI::Text::allTextboxMetrics;
std::vector<gl::GUI::Text::TextStyle> gl::GUI::Text::allTextStyles;
std::vector<gl::GUI::Text::String> gl::GUI::Text::allStrings;
std::vector<unsigned char> gl::GUI::Text::allChars;
std::vector<unsigned int> gl::GUI::Text::textboxStringIndices;
std::vector<unsigned int> gl::GUI::Text::fontStringIndices;
std::vector<gl::GUI::Text::CharQuad> gl::GUI::Text::charQuadBuffer;
std::vector<unsigned int> gl::GUI::Text::glyphIndexBuffer;
unsigned int gl::GUI::Text::styleStorage = 0;
std::vector<glm::vec2> gl::GUI::Text::allTextboxPositions;
std::vector<glm::vec2> gl::GUI::Text::allTextboxSizes;
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

void gl::GUI::Text::setStringColor(unsigned int pStringIndex, unsigned int pColorIndex)
{
	String& string = allStrings[pStringIndex];
	string.color = pColorIndex;
}

void gl::GUI::Text::setStringStyle(unsigned int pStringIndex, unsigned int pStyleIndex)
{
	String& string = allStrings[pStringIndex];
	string.style = pStyleIndex;
}



void gl::GUI::Text::
appendTextboxString(unsigned int pTextbox, unsigned int pStringIndex)
{
	Textbox& tb = allTextboxes[pTextbox];
	if (!tb.stringCount) {
		tb.stringOffset = textboxStringIndices.size();
	}
	++tb.stringCount;
	textboxStringIndices.insert(textboxStringIndices.begin() + tb.stringOffset, pStringIndex);
}

void gl::GUI::Text::
appendTextboxString(unsigned int pTextbox, String pString)
{
	appendTextboxString(pTextbox, allStrings.size());
	allStrings.push_back(pString);
}

void gl::GUI::Text::
appendTextboxString(unsigned int pTextbox, std::string pString)
{
	appendTextboxString(pTextbox, String(pString));
}

void gl::GUI::Text::revalidateTextboxStringIndices()
{
	unsigned int off = 0;
	for (Textbox& tb : allTextboxes) {
		tb.stringOffset = off;
		off += tb.stringCount;
	}
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
	return 0;
}

void gl::GUI::Text::initStyleBuffer() {

	allTextStyles.reserve(2);
	createTextStyle(1.5f, 0.8f);
	createTextStyle(1.2f, 0.8f);
	styleStorage = VAO::createStorage(sizeof(TextStyle)*allTextStyles.size(), &allTextStyles[0], 0);
	
	Shader::bindUniformBufferToShader(glyphShapeProgram, styleStorage, "StyleBuffer");
}

void gl::GUI::Text::
loadChars()
{
	for (unsigned int t = 0; t < allTextboxes.size(); ++t) {
		Textbox& tb = allTextboxes[t];
		//get total char count
		unsigned int total_char_count = 0;
		for (unsigned int s = 0; s < tb.stringCount; ++s) {
			total_char_count += allStrings[textboxStringIndices[tb.stringOffset + s]].charCount;
		}
		TextboxGlyphs tbGlyphs(total_char_count);

		loadTextboxGlyphs(tb, tbGlyphs);
		transformTextboxGlyphs(tb, tbGlyphs);

		unsigned int buffer_offset = charQuadBuffer.size();
		
		charQuadBuffer.resize(buffer_offset + tbGlyphs.quads.size());
		std::memcpy(&charQuadBuffer[buffer_offset], &tbGlyphs.quads[0], sizeof(CharQuad)*tbGlyphs.quads.size());
		
		glyphIndexBuffer.resize(buffer_offset + tbGlyphs.glyphIndices.size());
		std::memcpy(&glyphIndexBuffer[buffer_offset], &tbGlyphs.glyphIndices[0], sizeof(unsigned int)*tbGlyphs.glyphIndices.size());
	}
}

void gl::GUI::Text::
loadTextboxGlyphs(Textbox& pTextbox, TextboxGlyphs& pGlyphs)
{
	glm::vec2 cursor = glm::vec2();
	TextboxMetrics& textMetrics = allTextboxMetrics[pTextbox.metrics];
	
	//lines
	pGlyphs.line_sizes.reserve(pTextbox.stringCount);
	pGlyphs.lines.reserve(pTextbox.stringCount);
	
	glm::vec2 thisLineSize = glm::vec2();
	String thisLineStr = String(0, 0);
	unsigned int str_char_off = 0;
	unsigned int line_char_off = 0;
	for (unsigned int s = 0; s < pTextbox.stringCount; ++s) {
		
		unsigned int leftIndex = 0;
		unsigned int rightIndex = 0;
		unsigned int nonCharCount = 0;
		const unsigned int stringIndex = textboxStringIndices[pTextbox.stringOffset + s];
		String& str = allStrings[stringIndex];
		Font& font = allFonts[textMetrics.font];
		FontInstructions& font_inst = allFontInstructions[font.instructions];
		if (!font.stringCount) {
			font.stringOffset = fontStringIndices.size();
		}
		++font.stringCount;
		fontStringIndices.insert(fontStringIndices.begin() + font.stringOffset, stringIndex);
		
		//iterate string chars
		for (unsigned int c = 0; c < str.charCount; ++c) {
			//get glyph metrics
			unsigned int charCode = allChars[str.charOffset + c];
			rightIndex = std::max((unsigned int)0, std::min(charCode - font_inst.startCode, font_inst.glyphCount - 1));
			GlyphMetrics& met = allMetrics[font.metricOffset + rightIndex];
			//if newline
			if (charCode != '\n' && (cursor.x + met.advanceX * textMetrics.advanceScale) < (allTextboxSizes[pTextbox.size].x - pTextbox.marging*2.0f) ) {

				CharQuad qd(cursor.x + met.bearingX, cursor.y + met.bearingY, met.width * textMetrics.glyphScale.x, met.height * textMetrics.glyphScale.y);
				thisLineSize.y = std::max(thisLineSize.y, qd.size.y);
				pGlyphs.quads[str_char_off + c - nonCharCount] = qd;
				pGlyphs.glyphIndices[str_char_off + c - nonCharCount] = rightIndex;

				cursor.x += met.advanceX * textMetrics.advanceScale;
			}
			else {
				if (charCode == '\n') {
					leftIndex = 0;
					++nonCharCount;
				}
				//thisLineSize.y = font.fontMetric.lineGap;
				thisLineSize.x = cursor.x;
				thisLineStr = String(line_char_off, (str_char_off + c - nonCharCount) - line_char_off);
				pGlyphs.line_sizes.push_back(thisLineSize);
				pGlyphs.lines.push_back(thisLineStr);
				
				cursor = glm::vec2(0.0f, 0.0f);
				line_char_off = str_char_off + c - nonCharCount;
			}
			leftIndex = rightIndex;
		}
		str_char_off += str.charCount;
		if (s + 1 == pTextbox.stringCount) {
			thisLineSize.y = font.fontMetric.lineGap;
			thisLineSize.x = cursor.x;
			thisLineStr = String(line_char_off, str_char_off - line_char_off - nonCharCount);
			pGlyphs.line_sizes.push_back(thisLineSize);
			pGlyphs.lines.push_back(thisLineStr);
			cursor = glm::vec2(0.0f, 0.0);
		}
		
	}
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
	if (pTextbox.flags & TEXT_LAYOUT_CENTER_Y) {
		cursor.y = (cursor.y - (tb_size.y / 2.0f)) + allLineHeight/2.0f;
	}
	for (unsigned int l = 0; l < pGlyphs.lines.size(); ++l) {
		String& line = pGlyphs.lines[l];
		glm::vec2& line_size = pGlyphs.line_sizes[l];
		cursor.y -= font.fontMetric.lineGap;
		if (pTextbox.flags & TEXT_LAYOUT_CENTER_X) {
			cursor.x = tb_pos.x + (tb_size.x / 2.0f) - line_size.x / 2.0f;
		}
		else if (pTextbox.flags & TEXT_LAYOUT_BOUND_LEFT) {
			cursor.x = tb_pos.x + pTextbox.marging;
		}
		else if (pTextbox.flags & TEXT_LAYOUT_BOUND_RIGHT) {
			cursor.x = (tb_pos.x + tb_size.x) - (line_size.x + pTextbox.marging);
		}
		for (unsigned int g = 0; g < line.charCount; ++g) {
			CharQuad& qd = pGlyphs.quads[line.charOffset + g];
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

		Shader::setUniform(GUI::Text::glyphShapeProgram, "atlas", (int)0);
		//VAO::setUniform(glyphShapeProgram, "ortho", ortho);
		
		for (unsigned int fo = 0; fo < allFonts.size(); ++fo) {
			Font& font = allFonts[fo];
			glBindTexture(GL_TEXTURE_2D, font.atlasID);
			Shader::bindUniformBufferToShader(glyphShapeProgram, font.glyphStorageIndex, "GlyphBuffer");
			
			for (unsigned int s = 0; s < font.stringCount; ++s) {
				String& str = allStrings[fontStringIndices[font.stringOffset + s]];
				Shader::setUniform(glyphShapeProgram, "styleIndex", (unsigned int)str.style);
				
				glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, str.charCount, str.charOffset);

			}
		}
		Debug::getGLError("renderGlyphs():");
		Shader::unuse();
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}

gl::GUI::Text::String::String(std::string pString, unsigned int pColor, unsigned int pStyle)
:charOffset(allChars.size()), charCount(pString.size()), color(pColor), style(pStyle) {
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

}
