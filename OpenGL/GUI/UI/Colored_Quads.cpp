#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "GUI.h"
#include "Colored_Quads.h"
#include "../../BaseGL/VAO.h"
#include "../../BaseGL/Shader/Shader.h"
#include "../../Global/gl.h"
struct Colored_Quad {
	Colored_Quad(size_t pQuad, size_t pColor)
		:quad(pQuad), color(pColor) {}
	size_t quad;
	size_t color;
};

std::vector<Colored_Quad> allColoredQuads;
size_t coloredQuadBuffer;
size_t coloredQuadVAO;
size_t coloredQuadShader;

void gl::GUI::colorQuad(size_t pQuad, size_t pColor) {
	allColoredQuads.emplace_back(pQuad - 1, pColor);
}
void gl::GUI::renderColoredQuads() {
	glBindVertexArray(coloredQuadVAO);
	Shader::use(coloredQuadShader);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allColoredQuads.size());

	Shader::unuse();
	glBindVertexArray(0);
}
void gl::GUI::setupColoredQuadShader() {
	Shader::bindUniformBufferToShader(coloredQuadShader, GUI::colorBuffer, "ColorBuffer");
	Shader::bindUniformBufferToShader(coloredQuadShader, GUI::quadBuffer, "QuadBuffer");
}
void gl::GUI::initColoredQuadVAO() {

	coloredQuadBuffer = VAO::createStorage(sizeof(Colored_Quad)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(coloredQuadBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &coloredQuadVAO);
	glVertexArrayElementBuffer(coloredQuadVAO, quadEBO + 1);
	glVertexArrayVertexBuffer(coloredQuadVAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(coloredQuadVAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(coloredQuadVAO, 1, coloredQuadBuffer, sizeof(glm::uvec2));
	VAO::setVertexAttrib(coloredQuadVAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(coloredQuadVAO, 1, 1);
}

void gl::GUI::initColoredQuadShader() {
	coloredQuadShader = Shader::newProgram("coloredQuadShader", Shader::createModule("coloredQuadShader.vert"), Shader::createModule("coloredQuadShader.frag"));
	Shader::addVertexAttribute(coloredQuadShader, "corner_pos", 0);
	Shader::addVertexAttribute(coloredQuadShader, "colored_quad", 1);
}

void gl::GUI::updateColoredQuads()
{
	if (allColoredQuads.size()) {
		VAO::streamStorage(coloredQuadBuffer, sizeof(Colored_Quad)*allColoredQuads.size(), &allColoredQuads[0]);
	}
}