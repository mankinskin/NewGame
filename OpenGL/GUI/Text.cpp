#include "..\Global\stdafx.h"
#include "Text.h"
#include "..\BaseGL\Shader.h"
#include "..\Global\glDebug.h"
#include <algorithm>
#include "..\Camera.h"
#include <App\Global\Debug.h>
#include <glm\gtc\matrix_transform.hpp>
#include "../BaseGL/Framebuffer.h"



struct TextboxMetrics {
	TextboxMetrics(unsigned int pFont, glm::vec2 pGlyphScale, float pAdvanceScale, float pLineGapScale)
		:font(pFont), glyphScale(pGlyphScale), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
	TextboxMetrics(unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
		:font(pFont), glyphScale(glm::vec2(pGlyphScaleX, pGlyphScaleY)), advanceScale(pAdvanceScale), lineGapScale(pLineGapScale) {}
	unsigned int font;
	glm::vec2 glyphScale;
	float advanceScale;
	float lineGapScale;
};


struct Textbox {
	Textbox()
		:metrics(0), chars(gl::GUI::Text::String()), pos(0), size(0), marging(0.0f), flags(0) {}

	gl::GUI::Text::String chars;
	unsigned int metrics;
	unsigned int pos;
	unsigned int size;
	float marging = 0.0f;
	unsigned int flags;
};



struct TextColor {
	TextColor()
		:color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {}
	TextColor(float r, float g, float b, float a)
		:color(glm::vec4(r, g, b, a)) {}
	TextColor(glm::vec4& pColor)
		:color(pColor) {}
	glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};



struct TextboxGlyphs {
	TextboxGlyphs(unsigned int pGlyphCount) {
		quads.resize(pGlyphCount);
		glyphIndices.resize(pGlyphCount);
		lines.reserve(pGlyphCount);
		line_sizes.reserve(pGlyphCount);
	}
	std::vector<gl::GUI::Text::String> lines;
	std::vector<glm::vec2> line_sizes;
	std::vector<unsigned int> glyphIndices;
	std::vector<gl::GUI::Text::CharQuad> quads;
};

std::vector<Textbox> allTextboxes;
std::vector<TextboxMetrics> allTextboxMetrics;
std::vector<unsigned int> textboxStringIndices;
std::vector<glm::vec2> allTextboxPositions;
std::vector<glm::vec2> allTextboxSizes;
std::vector<glm::vec4> allTextColors;

std::vector<gl::GUI::Text::String> gl::GUI::Text::allFontStrings;
std::vector<gl::GUI::Text::CharQuad> gl::GUI::Text::charQuadBuffer;
std::vector<unsigned char> gl::GUI::Text::allChars;

//std::vector<std::string> gl::GUI::Text::allTextColorNames;


void gl::GUI::Text::reserveTextboxSpace(unsigned int pCount) {

	allTextboxes.reserve(pCount);
	allTextboxPositions.reserve(pCount);
	allTextboxSizes.reserve(pCount);
}

unsigned int gl::GUI::Text::createTextbox(unsigned int pPosIndex, unsigned int pSizeIndex, unsigned int pMetrics, int pFlags, float pMarging) {
	Textbox tb;
	tb.pos = pPosIndex;
	tb.size = pSizeIndex;
	tb.metrics = pMetrics;
	tb.marging = pMarging;
	tb.flags = pFlags;
	allTextboxes.push_back(tb);
	return allTextboxes.size() - 1;
}

unsigned int gl::GUI::Text::createTextbox(unsigned int pQuad, unsigned int pMetrics, int pFlags, float pMarging) {
	glm::vec4 qd = allQuads[pQuad];
	return createTextbox(qd, pMetrics, pFlags, pMarging);
}

unsigned int gl::GUI::Text::createTextbox(glm::vec4 pQuad, unsigned int pMetrics, int pFlags, float pMarging)
{
        allTextboxPositions.push_back(glm::vec2(pQuad.x, pQuad.y));
        allTextboxSizes.push_back(glm::vec2(pQuad.z, pQuad.w));
        return createTextbox(allTextboxPositions.size() - 1, allTextboxSizes.size() - 1, pMetrics, pFlags, pMarging);
}

unsigned int gl::GUI::Text::createTextbox(glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging)
{
	allTextboxPositions.push_back(pTopLeft);
	allTextboxSizes.push_back(pSize);
	return createTextbox(allTextboxPositions.size() - 1, allTextboxSizes.size() - 1, pMetrics, pFlags, pMarging);
}

void gl::GUI::Text::setTextboxString(unsigned int pTextbox, String pString)
{
	Textbox& tb = allTextboxes[pTextbox];
	tb.chars = pString;
}
void gl::GUI::Text::setTextboxString(unsigned int pTextbox, std::string pString)
{
	setTextboxString(pTextbox, String(pString));
}

void gl::GUI::Text::revalidateTextboxCharIndices()
{
	unsigned int off = 0;
	for (Textbox& tb : allTextboxes) {
		tb.chars.offset = off;
		off += tb.chars.count;
	}
}

unsigned int gl::GUI::Text::createTextboxMetrics(unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale)
{
	allTextboxMetrics.push_back(TextboxMetrics(pFont, glm::vec2(pGlyphScaleX, pGlyphScaleY), pAdvanceScale, pLineGapScale));
	return allTextboxMetrics.size() - 1;
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


void loadTextboxGlyphs(Textbox& pTextbox, TextboxMetrics& pTextMetrics, gl::GUI::Text::Font& pFont, TextboxGlyphs& pGlyphs)
{
	float cursor = 0.0f;
	gl::GUI::Text::FontInstructions& font_inst = gl::GUI::Text::allFontInstructions[pFont.instructions];
	
	glm::vec2 thisLineSize = glm::vec2();
	gl::GUI::Text::String thisLineStr = gl::GUI::Text::String(0, 0);
	unsigned int str_char_off = 0; //the offset of the current string (in pGlyphs.glyphIndices & pGlyphs.quads)
	unsigned int line_char_off = 0;//the offset of the current line
	unsigned int lineCharCount = 0;
	float thisLineGreatestAscend = 0.0f;
	float thisLineGreatestDescend = 0.0f;
	
	unsigned int str_nonCharCount = 0;
	//iterate string chars
	for (unsigned int c = 0; c < pTextbox.chars.count;) {
		//get glyph metrics
		unsigned int charCode = gl::GUI::Text::allChars[pTextbox.chars.offset + c];
		unsigned int glyphIndex = std::max((unsigned int)0, std::min(charCode - font_inst.startCode, font_inst.glyphCount - 1));
		gl::GUI::Text::GlyphMetrics& met = gl::GUI::Text::allMetrics[pFont.metricOffset + glyphIndex];
		
		if (charCode != '\n' && (cursor + met.advanceX * pTextMetrics.advanceScale) < (allTextboxSizes[pTextbox.size].x - pTextbox.marging*2.0f) ) {
			//append char to all chars
			gl::GUI::Text::CharQuad qd(cursor+ met.bearingX, met.bearingY, met.width * pTextMetrics.glyphScale.x, met.height * pTextMetrics.glyphScale.y);
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

			thisLineStr = gl::GUI::Text::String(line_char_off, lineCharCount);
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

	thisLineStr = gl::GUI::Text::String(line_char_off, lineCharCount);
	pGlyphs.lines.push_back(thisLineStr);
	pGlyphs.line_sizes.shrink_to_fit();
	pGlyphs.lines.shrink_to_fit();
	pGlyphs.glyphIndices.resize(str_char_off);
	pGlyphs.quads.resize(str_char_off);
}
void transformTextboxGlyphs(Textbox& pTextbox, TextboxGlyphs& pGlyphs)
{
	glm::vec2 tb_pos = allTextboxPositions[pTextbox.pos];
	glm::vec2 tb_size = allTextboxSizes[pTextbox.size];
	TextboxMetrics& textMetrics = allTextboxMetrics[pTextbox.metrics];
	gl::GUI::Text::Font& font = gl::GUI::Text::allFonts[textMetrics.font];
	
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
		gl::GUI::Text::String& line = pGlyphs.lines[l];
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
			gl::GUI::Text::CharQuad& qd = pGlyphs.quads[line.offset + g];
			qd.pos += cursor;
		}
		
	}

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

void gl::GUI::Text::renderGlyphs()
{
	if (allTextboxes.size()) {
		//glBindFramebuffer(GL_FRAMEBUFFER, Texture::fontFBO);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(fontVAO);
		Shader::use(glyphShapeProgram);

		glActiveTexture(GL_TEXTURE0);
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
		Debug::getGLError("renderGlyphs():");
		Shader::unuse();
		glBindVertexArray(0);
	}
}

gl::GUI::Text::String::String(std::string pString)
:offset(allChars.size()), count(pString.size()) {
	allChars.insert(allChars.end(), pString.begin(), pString.end());
}


