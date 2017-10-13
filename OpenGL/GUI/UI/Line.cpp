#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "Line.h"
#include "../../BaseGL/VAO.h"
#include "../../BaseGL/Shader/Shader.h"

std::vector<glm::vec4> allLines;
std::vector<size_t> allLineColorIndices;
size_t guiLineVAO = 0;
size_t guiLineVBO = 0;
size_t guiLineColorIndexVBO = 0;
size_t guiLineShader = 0;
size_t MAX_GUI_LINES = 1000;

//size_t gl::GUI::createLine(glm::vec2 pV1, glm::vec2 pV2, size_t pColorIndex) {
//	allLines.push_back(glm::vec4(pV1, pV2));
//	allLineColorIndices.push_back(pColorIndex);
//	return allLines.size() - 1;
//}

void gl::GUI::setLineColor(size_t pLineIndex, size_t pColorIndex)
{
	allLineColorIndices[pLineIndex] = pColorIndex;
}

void gl::GUI::initLineVAO()
{
	glCreateVertexArrays(1, &guiLineVAO);

	guiLineVBO = VAO::createStorage(sizeof(glm::vec4) * MAX_GUI_LINES, nullptr, VAO::STREAM_FLAGS | GL_MAP_WRITE_BIT);
	VAO::createStream(guiLineVBO, GL_MAP_WRITE_BIT);

	guiLineColorIndexVBO = VAO::createStorage(sizeof(size_t) * MAX_GUI_LINES, nullptr, VAO::STREAM_FLAGS | GL_MAP_WRITE_BIT);
	VAO::createStream(guiLineColorIndexVBO, GL_MAP_WRITE_BIT);

	VAO::setVertexArrayVertexStorage(guiLineVAO, 0, guiLineVBO, sizeof(glm::vec4));
	VAO::setVertexArrayVertexStorage(guiLineVAO, 1, guiLineColorIndexVBO, sizeof(size_t));
	VAO::setVertexAttrib(guiLineVAO, 0, 0, 4, GL_FLOAT, 0);
	VAO::setVertexAttrib(guiLineVAO, 1, 1, 1, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(guiLineVAO, 0, 1);
	glVertexArrayBindingDivisor(guiLineVAO, 1, 1);
}

void gl::GUI::updateLines() {
	if (allLines.size()) {
		VAO::streamStorage(guiLineVBO, sizeof(glm::vec4) * allLines.size(), &allLines[0]);
		VAO::streamStorage(guiLineColorIndexVBO, sizeof(size_t) * allLineColorIndices.size(), &allLineColorIndices[0]);
	}
}

void gl::GUI::setupLineShader()
{
	Shader::bindUniformBufferToShader(guiLineShader, GUI::colorBuffer, "ColorBuffer");
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
	guiLineShader = Shader::newProgram("guiLineShader", Shader::createModule("guiLineShader.vert"), Shader::createModule("guiLineShader.frag"));
	Shader::addVertexAttribute(guiLineShader, "vertex_pair", 0);
	Shader::addVertexAttribute(guiLineShader, "color_index", 1);
}

