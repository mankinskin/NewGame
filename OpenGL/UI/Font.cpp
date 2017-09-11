#include "../Global/stdafx.h"
#include "Font.h"
#include "Text.h"
#include "../BaseGL/Shader.h"

unsigned int gl::GUI::Text::glyphShapeProgram = 0;
unsigned int gl::GUI::Text::MAX_CHARS = 1000;

std::vector<gl::GUI::Text::Font> gl::GUI::Text::allFonts;
unsigned int gl::GUI::Text::quadStorage = 0;
unsigned int gl::GUI::Text::charStorage = 0;

std::vector<float> gl::GUI::Text::allKerning;
std::vector<gl::GUI::Text::GlyphMetrics> gl::GUI::Text::allMetrics;
unsigned int gl::GUI::Text::fontVAO = 0;
std::vector<gl::GUI::Text::FontInstructions> gl::GUI::Text::allFontInstructions;

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
	quadStorage = VAO::createStorage(MAX_CHARS * sizeof(CharQuad), nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(quadStorage, GL_MAP_WRITE_BIT);
	VAO::bindVertexArrayVertexStorage(fontVAO, 1, quadStorage, sizeof(CharQuad));
	VAO::initVertexAttrib(fontVAO, 1, 1, 4, GL_FLOAT, 0);

	//glyph index
	charStorage = VAO::createStorage(MAX_CHARS * sizeof(unsigned int), nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(charStorage, GL_MAP_WRITE_BIT);
	VAO::bindVertexArrayVertexStorage(fontVAO, 2, charStorage, sizeof(unsigned int));
	VAO::initVertexAttrib(fontVAO, 2, 2, 1, GL_UNSIGNED_INT, 0);

	glVertexArrayBindingDivisor(fontVAO, 1, 1);
	glVertexArrayBindingDivisor(fontVAO, 2, 1);

	glVertexArrayElementBuffer(fontVAO, quadEBO);
}