#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "Font.h"
#include "Text.h"
#include "../../BaseGL/Shader/Shader.h"


size_t MAX_CHARS = 1000;


size_t quadStorage = 0;
unsigned charStorage = 0;
std::vector<gl::GUI::Text::String> allStrings;




size_t styleStorage = 0;
std::vector<gl::GUI::Text::TextStyle> allTextStyles;


size_t gl::GUI::Text::fontVAO = 0;
size_t gl::GUI::Text::glyphShaderProgram = 0;
std::vector<size_t> gl::GUI::Text::glyphIndexBuffer;
std::vector<gl::GUI::Text::Font> gl::GUI::Text::allFonts;
std::vector<gl::GUI::Text::FontInstructions> gl::GUI::Text::allFontInstructions;
std::vector<gl::GUI::Text::GlyphMetrics> gl::GUI::Text::allMetrics;
std::vector<float> gl::GUI::Text::allKerning;

void gl::GUI::Text::
initFontShader()
{
    glyphShaderProgram = Shader::newProgram("glyphShapeShader", Shader::createModule("glyphShapeShader.vert"), Shader::createModule("glyphShapeShader.frag"));
    Shader::addVertexAttribute(glyphShaderProgram, "pos", 0);
    Shader::addVertexAttribute(glyphShaderProgram, "quad", 1);
    Shader::addVertexAttribute(glyphShaderProgram, "index", 2);
}

void gl::GUI::Text::
initFontVAO() {
    quadStorage = VAO::createStorage(MAX_CHARS * sizeof(CharQuad), nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
    charStorage = VAO::createStorage(MAX_CHARS * sizeof(size_t), nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
    VAO::createStream(quadStorage, GL_MAP_WRITE_BIT);
    VAO::createStream(charStorage, GL_MAP_WRITE_BIT);
    glCreateVertexArrays(1, &fontVAO);

    VAO::setVertexAttrib(fontVAO, 0, 0, 2, GL_FLOAT, 0);
    VAO::setVertexAttrib(fontVAO, 1, 1, 4, GL_FLOAT, 0);
    VAO::setVertexAttrib(fontVAO, 2, 2, 1, GL_UNSIGNED_INT, 0);

    glVertexArrayBindingDivisor(fontVAO, 1, 1);
    glVertexArrayBindingDivisor(fontVAO, 2, 1);

    glVertexArrayElementBuffer(fontVAO, quadEBO + 1);
    glVertexArrayVertexBuffer(fontVAO, 0, quadVBO + 1, 0, sizeof(float) * 2);

    VAO::setVertexArrayVertexStorage(fontVAO, 1, quadStorage, sizeof(CharQuad));
    VAO::setVertexArrayVertexStorage(fontVAO, 2, charStorage, sizeof(size_t));
}

void gl::GUI::Text::revalidateFontStringIndices()
{
    size_t off = 0;
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
size_t gl::GUI::Text::createTextStyle(TextStyle & pStyle)
{
    allTextStyles.push_back(pStyle);
    return allTextStyles.size() - 1;
}
void gl::GUI::Text::initStyleBuffer() {

    allTextStyles.reserve(2);
    createTextStyle(1.5f, 0.8f);
    createTextStyle(1.2f, 0.8f);
    styleStorage = VAO::createStorage(sizeof(TextStyle)*allTextStyles.size(), &allTextStyles[0], 0);

    Shader::bindUniformBufferToShader(glyphShaderProgram, styleStorage, "StyleBuffer");
}

void gl::GUI::Text::updateCharStorage()
{
    if (charQuadBuffer.size()) {
	VAO::streamStorage(quadStorage, sizeof(CharQuad)*charQuadBuffer.size(), &charQuadBuffer[0]);
	VAO::streamStorage(charStorage, sizeof(size_t)*glyphIndexBuffer.size(), &glyphIndexBuffer[0]);
    }
}

void gl::GUI::Text::clearCharStorage()
{
    for (Font& font : allFonts) {
	font.stringOffset = 0;
	font.stringCount = 0;
    }
    allChars.clear();
    glyphIndexBuffer.clear();
    charQuadBuffer.clear();
    allStrings.clear();
    allFontStrings.clear();
}