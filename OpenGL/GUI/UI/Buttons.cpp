#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "Buttons.h"
#include "../../Global/glDebug.h"
#include "../../BaseGL/Framebuffer.h"
#include "../../BaseGL/Shader/Shader.h"
#include "../../BaseGL/VAO.h"
#include <App/Input/Mouse.h>

size_t buttonVAO;
size_t buttonBuffer;
size_t buttonIndexShader;
std::vector<glm::uvec2> allButtons;
std::vector<size_t> buttonIndexMap;
size_t BUTTON_ID_COUNT = 0;

void gl::GUI::rasterButtonQuads() {
	if (allButtons.size()) {
		glBindVertexArray(buttonVAO);
		gl::Shader::use(buttonIndexShader);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allButtons.size());

		gl::Shader::unuse();
		glBindVertexArray(0);

		Debug::getGLError("rasterButtonQuads()");
	}
}

void gl::GUI::initButtonIndexShader() {
	buttonIndexShader = gl::Shader::newProgram("buttonIndexShader", gl::Shader::createModule("buttonIndexShader.vert"), gl::Shader::createModule("buttonIndexShader.frag"));
	gl::Shader::addVertexAttribute(buttonIndexShader, "corner_pos", 0);
	gl::Shader::addVertexAttribute(buttonIndexShader, "quad_index", 1);
	gl::Shader::addVertexAttribute(buttonIndexShader, "button_index", 2);
}

void gl::GUI::rasterButtons()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gl::Texture::guiFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(0);
	rasterButtonQuads();
	//rasterButtonLines();
	glDepthMask(1);
}

void gl::GUI::initButtonBuffer()
{
	buttonBuffer = gl::VAO::createStorage(sizeof(glm::uvec2) * 10000, 0, GL_MAP_WRITE_BIT | gl::VAO::STREAM_FLAGS);
	gl::VAO::createStream(buttonBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &buttonVAO);
	glVertexArrayElementBuffer(buttonVAO, gl::quadEBO + 1);
	glVertexArrayVertexBuffer(buttonVAO, 0, gl::quadVBO + 1, 0, sizeof(glm::vec2));
	gl::VAO::setVertexAttrib(buttonVAO, 0, 0, 2, GL_FLOAT, 0);

	gl::VAO::setVertexArrayVertexStorage(buttonVAO, 1, buttonBuffer, sizeof(glm::uvec2));
	gl::VAO::setVertexAttrib(buttonVAO, 1, 1, 1, GL_UNSIGNED_INT, 0);
	gl::VAO::setVertexAttrib(buttonVAO, 1, 2, 1, GL_UNSIGNED_INT, sizeof(unsigned int));
	glVertexArrayBindingDivisor(buttonVAO, 1, 1);
	buttonIndexMap.resize(gl::screenWidth * gl::screenHeight);
}

void gl::GUI::updateButtonBuffer()
{
	if (allButtons.size()) {
		gl::VAO::streamStorage(buttonBuffer, sizeof(glm::uvec2)*allButtons.size(), &allButtons[0]);
	}
}

size_t gl::GUI::addButton(size_t pQuadID)
{
	allButtons.emplace_back(pQuadID, BUTTON_ID_COUNT++);
	return BUTTON_ID_COUNT - 1;
}

size_t gl::GUI::addButton(size_t pQuadID, size_t pButtonID)
{
	allButtons.emplace_back(pQuadID, pButtonID);
	return pButtonID;
}

void gl::GUI::setupButtonIndexShader()
{
	Shader::bindUniformBufferToShader(buttonIndexShader, quadBuffer, "QuadBuffer");
}

void gl::GUI::fetchButtonMap()
{
	//get button id pixels
	glGetTextureImage(gl::Texture::buttonIndexTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, gl::screenWidth * gl::screenHeight, &buttonIndexMap[0]);
}
void gl::GUI::clearButtons()
{
	allButtons.clear();
}

size_t gl::GUI::readButtonIndexMap(size_t pPos) {
	return buttonIndexMap[pPos];
}
size_t gl::GUI::readButtonIndexMap(size_t pXPos, size_t pYPos) {
	return readButtonIndexMap((gl::screenWidth * pYPos) + pXPos);
}