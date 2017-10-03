#include "../Global/stdafx.h"
#include "Line.h"
#include "../BaseGL/VAO.h"
#include "../BaseGL/Shader.h"
std::vector<glm::vec4> gl::GUI::allLines;
std::vector<unsigned int> gl::GUI::allLineColorIndices;
unsigned int gl::GUI::guiLineVAO = 0;
unsigned int gl::GUI::guiLineVBO = 0;
unsigned int gl::GUI::guiLineColorIndexVBO = 0;
unsigned int gl::GUI::guiLineShader = 0;
unsigned int gl::GUI::MAX_GUI_LINES = 1000;


unsigned int gl::GUI::createLine(glm::vec2 pV1, glm::vec2 pV2, unsigned int pColorIndex) {
	allLines.push_back(glm::vec4(pV1, pV2));
	allLineColorIndices.push_back(pColorIndex);
	return allLines.size() - 1;
}

void gl::GUI::setLineVertex(unsigned int pLineIndex, unsigned int pVertexOffset, glm::vec2 pVertex)
{
	std::memcpy(&allLines[pLineIndex][pVertexOffset * 2], &pVertex[0], sizeof(float) * 2);
}

void gl::GUI::setLineColor(unsigned int pLineIndex, unsigned int pColorIndex)
{
	allLineColorIndices[pLineIndex] = pColorIndex;
}

void gl::GUI::initLineVAO()
{
	glCreateVertexArrays(1, &guiLineVAO);

	guiLineVBO = VAO::createStorage(sizeof(glm::vec4) * MAX_GUI_LINES, nullptr, VAO::STREAM_FLAGS | GL_MAP_WRITE_BIT);
	VAO::createStream(guiLineVBO, GL_MAP_WRITE_BIT);
	
	guiLineColorIndexVBO = VAO::createStorage(sizeof(unsigned int) * MAX_GUI_LINES, nullptr, VAO::STREAM_FLAGS | GL_MAP_WRITE_BIT);
	VAO::createStream(guiLineColorIndexVBO, GL_MAP_WRITE_BIT);
	
	VAO::setVertexArrayVertexStorage(guiLineVAO, 0, guiLineVBO, sizeof(glm::vec4));
	VAO::setVertexArrayVertexStorage(guiLineVAO, 1, guiLineColorIndexVBO, sizeof(unsigned int));
	VAO::setVertexAttrib(guiLineVAO, 0, 0, 4, GL_FLOAT, 0);
	VAO::setVertexAttrib(guiLineVAO, 1, 1, 1, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(guiLineVAO, 0, 1);
	glVertexArrayBindingDivisor(guiLineVAO, 1, 1);
}

void gl::GUI::updateLines() {
	if (allLines.size()) {
		VAO::streamStorage(guiLineVBO, sizeof(glm::vec4) * allLines.size(), &allLines[0]);
		VAO::streamStorage(guiLineColorIndexVBO, sizeof(unsigned int) * allLineColorIndices.size(), &allLineColorIndices[0]);
	}
}

void gl::GUI::renderLines()
{
	glBlendFunc(GL_ONE, GL_ONE);
	glBindVertexArray(guiLineVAO);
	Shader::use(guiLineShader);
	glDrawArraysInstanced(GL_LINES, 0, 2, allLines.size());
	Shader::unuse();
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void gl::GUI::initLineShader() {
	guiLineShader = Shader::newProgram("guiLineShader", Shader::newModule("guiLineShader.vert"), Shader::newModule("guiLineShader.frag")).ID;
	Shader::addVertexAttribute(guiLineShader, "vertex_pair", 0);
	Shader::addVertexAttribute(guiLineShader, "color_index", 1);
}
