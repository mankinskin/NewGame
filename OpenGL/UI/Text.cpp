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

void gl::GUI::Text::setStringFont(unsigned int pStringIndex, unsigned int pFontIndex)
{
	String& string = allStrings[pStringIndex];
	string.font = pFontIndex;
}

void gl::GUI::Text::setStringStyle(unsigned int pStringIndex, unsigned int pStyleIndex)
{
	String& string = allStrings[pStringIndex];
	string.style = pStyleIndex;
}

unsigned int
gl::GUI::Text::createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging)
{
	allTextboxPositions.push_back(pTopLeft);
	allTextboxSizes.push_back(pSize);
	return createTextbox(allTextboxPositions.size()-1, allTextboxSizes.size()-1, pMetrics, pFlags, pMarging);
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


unsigned int gl::GUI::Text::createTextboxMetrics(float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
{
	allTextboxMetrics.push_back(TextboxMetrics(glm::vec2(pGlyphScaleX, pGlyphScaleY), pAdvanceScale, pLineGapScale));
	return allTextboxMetrics.size() - 1;
}

unsigned int gl::GUI::Text::createTextStyle(TextStyle & pStyle)
{
	allTextStyles.push_back(pStyle);
	return allTextStyles.size() - 1;
}

unsigned int gl::GUI::Text::createTextStyle(float pThickness, float pHardness, glm::vec4 pFrontColor, glm::vec4 pBackColor)
{
	return createTextStyle(TextStyle(pThickness, pHardness, pFrontColor, pBackColor));
}

void gl::GUI::Text::initStyleBuffer() {

	allTextStyles.reserve(2);
	createTextStyle(1.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	createTextStyle(1.2f, 0.8f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	styleStorage = VAO::createStorage(sizeof(TextStyle)*allTextStyles.size(), &allTextStyles[0], 0);
	
	Shader::bindUniformBufferToShader(glyphShapeProgram, styleStorage, "StyleBuffer");
}

void gl::GUI::Text::
loadChars()
{

	for (unsigned int t = 0; t < allTextboxes.size(); ++t) {
		loadTextboxGlyphs(allTextboxes[t]);

	}
}

void gl::GUI::Text::
loadTextboxGlyphs(Textbox& pTextbox)
{
	glm::vec2 cursor = glm::vec2();
	glm::vec2 tb_pos = allTextboxPositions[pTextbox.pos];
	TextboxMetrics& textMetrics = allTextboxMetrics[pTextbox.metrics];
	//get total char count
	unsigned int total_char_count = 0;
	for (unsigned int s = 0; s < pTextbox.stringCount; ++s) {
		total_char_count += allStrings[textboxStringIndices[pTextbox.stringOffset + s]].charCount;
	}
	std::vector<unsigned int> glyphIndices(total_char_count);
	std::vector<CharQuad> quads(total_char_count);
	
	std::vector<String> lines;
	std::vector<float> line_widths;
	line_widths.resize(total_char_count);
	lines.resize(total_char_count);
	unsigned int line_count = 0;
	unsigned int buffer_off = 0;
	for (unsigned int s = 0; s < pTextbox.stringCount; ++s) {
		
		unsigned int leftIndex = 0;
		unsigned int rightIndex = 0;
		unsigned int nonCharCount = 0;
		const unsigned int stringIndex = textboxStringIndices[pTextbox.stringOffset + s];
		String& str = allStrings[stringIndex];
		Font& font = allFonts[str.font];
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
			if (charCode == '\n' || (cursor.x + met.advanceX * textMetrics.advanceScale) * !(pTextbox.flags & TEXT_LAYOUT_FORCE_X_FIT) >(allTextboxSizes[pTextbox.size].x - pTextbox.marging*2.0f)) {
				if (charCode == '\n') {
					leftIndex = 0;
					++nonCharCount;
				}
				//transforming,  do this somewhere before
				line_widths[line_count] = cursor.x;
				lines[line_count].charCount = (buffer_off + c) - lines[line_count].charOffset;
				lines[++line_count].charOffset = buffer_off + c;
				
				cursor = glm::vec2(0.0f, cursor.y - font.fontMetric.lineGap*textMetrics.lineGapScale);
			}
			else {
				glyphIndices[buffer_off + c - nonCharCount] = rightIndex;
				quads[buffer_off + c - nonCharCount] = CharQuad(tb_pos.x + pTextbox.marging + cursor.x + met.bearingX, tb_pos.y + cursor.y - font.fontMetric.lineGap*textMetrics.lineGapScale + met.bearingY, met.width * textMetrics.glyphScale.x, met.height * textMetrics.glyphScale.y);
				cursor.x += met.advanceX * textMetrics.advanceScale;
			}

			leftIndex = rightIndex;
		}
		buffer_off += str.charCount;
	}
	glyphIndices.resize(buffer_off);
	quads.resize(buffer_off);
	line_widths.resize(++line_count);
	lines.resize(line_count);
	//transformations here

	unsigned int buffer_offset = charQuadBuffer.size();
	charQuadBuffer.resize(buffer_offset + buffer_off);
	glyphIndexBuffer.resize(buffer_offset + buffer_off);

	std::memcpy(&charQuadBuffer[buffer_offset], &quads[0], sizeof(CharQuad)*buffer_off);
	std::memcpy(&glyphIndexBuffer[buffer_offset], &glyphIndices[0], sizeof(unsigned int)*buffer_off);

}


//void gl::GUI::Text::translateTextbox(Textbox & pTextbox)
//{
//	//transform glyphs
//	glm::vec2 cursor = *pTextbox.pos;
//	//force fit by 
//	float fit_x = 1.0f;
//	float fit_y = 1.0f;
//	if (!(pTextbox.flags ^ TEXT_LAYOUT_FORCE_X_FIT)) {
//		fit_x = std::min(1.0f, (pTextbox.size->x + pTextbox.marging*2.0f) / string_width);
//	}
//	if (!(pTextbox.flags ^ TEXT_LAYOUT_FORCE_Y_FIT)) {
//		fit_y = std::min(1.0f, pTextbox.size->y / string_height);
//	}
//
//	if (!(pTextbox.flags ^ TEXT_LAYOUT_CENTER)) {
//		cursor = glm::vec2(cursor.x + pTextbox.marging + (pTextbox.size->x / 2.0f) - string_width / 2.0f, cursor.y - (pTextbox.size->y / 2.0f) - string_height / 2.5f);
//	}
//	else if (!(pTextbox.flags ^ TEXT_LAYOUT_BOUND_LEFT)) {
//		cursor = glm::vec2(cursor.x + pTextbox.marging, cursor.y - (pTextbox.size->y / 2.0f) - string_height / 2.5f);
//	}
//	else if (!(pTextbox.flags ^ TEXT_LAYOUT_BOUND_RIGHT)) {
//		cursor = glm::vec2((cursor.x + pTextbox.size->x) - string_width + pTextbox.marging, cursor.y - (pTextbox.size->y / 2.0f) - string_height / 2.5f);
//	}
//	for (unsigned int s = 0; s < pTextbox.stringCount; ++s) {
//		String& string = allStrings[textboxStringIndices[pTextbox.stringOffset + s]];
//		std::vector<CharQuad> quads(string.charCount);//for positioning
//		for (unsigned int g = 0; g < string.charCount; ++g) {
//			unsigned char charindex = charIndices[g];
//			GlyphMetrics& met = allMetrics[charindex];//may be unsafe(?) look here for bugs ... ;D
//
//													  //float kern = allKerning[font.kerningOffset + leftIndex*inst.glyphCount + rightIndex] * textMetrics.advanceScale;
//			quads[g] = CharQuad(cursor.x + met.bearingX*textMetrics.advanceScale*fit_x, cursor.y + met.bearingY*textMetrics.glyphScale.y*fit_y, met.width*textMetrics.glyphScale.x*fit_x, met.height*textMetrics.glyphScale.y*fit_y);
//			cursor.x += met.advanceX * textMetrics.advanceScale * fit_x;
//
//		}
//	}
//
//	string.charOffset = charQuadBuffer.size();
//	string.charCount -= nonCharCount;
//}

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
				//Shader::setUniform(glyphShapeProgram, "styleIndex", (unsigned int)str.style);
				
				glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, str.charCount, str.charOffset);

			}
		}
		Debug::getGLError("renderGlyphs():");
		Shader::unuse();
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}

gl::GUI::Text::String::String(std::string pString, unsigned int pFont, unsigned int pStyle)
:charOffset(allChars.size()), charCount(pString.size()), font(pFont), style(pStyle) {
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
