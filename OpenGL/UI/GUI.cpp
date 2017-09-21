#include "..\Global\stdafx.h"
#include "GUI.h"
#include "..\BaseGL\Shader.h"
#include "..\BaseGL\VAO.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>

std::vector<glm::vec2> gl::GUI::allPositions;
std::vector<glm::vec2> gl::GUI::allSizes;
std::vector<glm::vec4> gl::GUI::allColors;

std::vector<gl::GUI::RefQuad> gl::GUI::allQuads;

unsigned int gl::GUI::MAX_GUI_QUADS = 10000;

unsigned int gl::GUI::guiVAO = 0;

unsigned int gl::GUI::guiQuadBuffer;
unsigned int gl::GUI::guiColorBuffer = 0;
unsigned int gl::GUI::guiPositionBuffer = 0;
unsigned int gl::GUI::guiSizeBuffer = 0;

unsigned int gl::GUI::guiTexQuadShader = 0;
unsigned int gl::GUI::guiQuadShader = 0;
std::vector<int> gl::GUI::allQuadFlags;


void gl::GUI::clearBuffers()
{
	allQuads.clear();
	allPositions.clear();
	allSizes.clear();
	allColors.clear();
}

void gl::GUI::setQuadVisibility(unsigned int pQuadIndex, int pHide) {
	allQuadFlags[pQuadIndex] = pHide;
}

gl::GUI::Quad gl::GUI::createQuad(Pos pPos, Size pSize, Color pColor)
{
	allQuads.push_back(RefQuad(pPos, pSize, pColor));
	allQuadFlags.push_back(1);
	return allQuads.size() - 1;
}

gl::GUI::Quad gl::GUI::createQuad(glm::vec2 pPos, glm::vec2 pSize, glm::vec4 pColor)
{
	unsigned int ret = createQuad(allPositions.size(), allSizes.size(), allColors.size());
	allPositions.push_back(pPos);
	allSizes.push_back(pSize);
	allColors.push_back(pColor);
	return ret;
}

gl::GUI::Quad gl::GUI::createQuad(float pPosX, float pPosY, float pWidth, float pHeight, float pR, float pG, float pB, float pA)
{
	return createQuad(glm::vec2(pPosX, pPosY), glm::vec2(pWidth, pHeight), glm::vec4(pR, pG, pB, pA));
}



void gl::GUI::reserveQuadSpace(unsigned int pCount)
{
	allQuads.reserve(pCount);
	allQuadFlags.reserve(pCount);
	allPositions.reserve(pCount);
	allSizes.reserve(pCount);
	allColors.reserve(pCount);
}

void gl::GUI::initGUIVAO()
{
	glCreateVertexArrays(1, &guiVAO);
	guiPositionBuffer = VAO::createStorage( MAX_GUI_QUADS*sizeof(float)*2, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	guiSizeBuffer = VAO::createStorage(MAX_GUI_QUADS * sizeof(float) * 2, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	guiColorBuffer = VAO::createStorage(MAX_GUI_QUADS * sizeof(float) * 4, nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	guiQuadBuffer = VAO::createStorage(MAX_GUI_QUADS * sizeof(glm::ivec4), nullptr, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);

	VAO::createStream(guiPositionBuffer, GL_MAP_WRITE_BIT);
	VAO::createStream(guiSizeBuffer, GL_MAP_WRITE_BIT);
	VAO::createStream(guiColorBuffer, GL_MAP_WRITE_BIT);
	VAO::createStream(guiQuadBuffer, GL_MAP_WRITE_BIT);

	VAO::bindStorage(GL_UNIFORM_BUFFER, guiColorBuffer);
	VAO::bindStorage(GL_UNIFORM_BUFFER, guiPositionBuffer);
	VAO::bindStorage(GL_UNIFORM_BUFFER, guiSizeBuffer);

	glVertexArrayElementBuffer(guiVAO, quadEBO+1);
	glVertexArrayVertexBuffer(guiVAO, 0, quadVBO+1, 0, sizeof(glm::vec2));
	VAO::setVertexArrayVertexStorage(guiVAO, 1, guiQuadBuffer, sizeof(glm::ivec4));
	

	glVertexArrayBindingDivisor(guiVAO, 1, 1);

	VAO::setVertexAttrib(guiVAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexAttrib(guiVAO, 1, 1, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, x));
	VAO::setVertexAttrib(guiVAO, 1, 2, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, y));
	VAO::setVertexAttrib(guiVAO, 1, 3, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, z));
	VAO::setVertexAttrib(guiVAO, 1, 4, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, w));
	
	gl::Debug::getGLError("gl::GUI::initGuiBuffers()");
}

void gl::GUI::updateGUI() {

	unsigned int quad_count = allQuads.size();
	if (!quad_count) {
		return;
	}
	
	std::vector<glm::ivec4> indexQuads(quad_count);
	std::vector<glm::ivec4>::iterator it = indexQuads.begin();
	for (unsigned int q = 0; q < quad_count; ++q) {
		if (allQuadFlags[q]) {
			RefQuad& quad = allQuads[q];
			*(it++) = glm::ivec4(
				&allPositions[quad.pos] - &allPositions[0], &allSizes[quad.size] - &allSizes[0],
				&allColors[quad.color] - &allColors[0], 0);
		}
	}


	VAO::streamStorage(guiQuadBuffer, sizeof(glm::ivec4)*quad_count, &indexQuads[0]);
	VAO::streamStorage(guiPositionBuffer, sizeof(glm::vec2)*quad_count, &allPositions[0]);
	VAO::streamStorage(guiSizeBuffer, sizeof(glm::vec2)*quad_count, &allSizes[0]);
	VAO::streamStorage(guiColorBuffer, sizeof(glm::vec4)*quad_count, &allColors[0]);
	//VAO::streamTargetStorage(guiUVBuffer, sizeof(glm::vec4)*allUVRanges.size(), &allUVRanges[0]);
}

void gl::GUI::initGUIShaders()
{
	
	guiQuadShader = Shader::newProgram("guiQuad", Shader::newModule("guiQuad.vert"), Shader::newModule("guiQuad.frag")).ID;
	Shader::addVertexAttribute(guiQuadShader, "posIndex", 0);
	Shader::addVertexAttribute(guiQuadShader, "sizeIndex", 1);
	Shader::addVertexAttribute(guiQuadShader, "uvIndex", 2);
	Shader::addVertexAttribute(guiQuadShader, "colorIndex", 3);
}

void gl::GUI::renderGUI()
{
	if (allQuads.size()) {
		glBindVertexArray(guiVAO);

		Shader::use(guiQuadShader);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allQuads.size());


		Shader::unuse();
		glBindVertexArray(0);
		Debug::getGLError("renderGUI");
	}
}

gl::GUI::Pos gl::GUI::createPos(glm::vec2 pPos)
{
	allPositions.push_back(pPos);
	return allPositions.size() - 1;
}

gl::GUI::Pos gl::GUI::createPos(float pX, float pY)
{
	return createPos(glm::vec2(pX, pY));
}

gl::GUI::Size gl::GUI::createSize(glm::vec2 pSize)
{
	allSizes.push_back(pSize);
	return allSizes.size() - 1;
}

gl::GUI::Size gl::GUI::createSize(float pW, float pH)
{
	return createSize(glm::vec2(pW, pH));
}

gl::GUI::Color gl::GUI::createColor(glm::vec4 pColor)
{
	allColors.push_back(pColor);
	return allColors.size()-1;
}

gl::GUI::Color gl::GUI::createColor(float pR, float pG, float pB, float pA)
{
	return createColor(glm::vec4(pR, pG, pB, pA));
}

void gl::GUI::changePos(Quad pQuad, const glm::vec2& pPos)
{
	allPositions[allQuads[pQuad].pos] = pPos;
}

void gl::GUI::changeSize(Quad pQuad, const glm::vec2& pSize)
{

	allSizes[allQuads[pQuad].size] = pSize;
}

void gl::GUI::changeColor(Quad pQuad, const glm::vec4& pColor)
{
	allColors[allQuads[pQuad].color] = pColor;
}

void gl::GUI::offsetPos(Quad pQuad, const glm::vec2& pPos)
{
	allPositions[allQuads[pQuad].pos] += pPos;
}

void gl::GUI::offsetSize(Quad pQuad, const glm::vec2& pSize)
{
	allSizes[allQuads[pQuad].size] += pSize;
}

void gl::GUI::offsetColor(Quad pQuad, const glm::vec4& pColor)
{
	allColors[allQuads[pQuad].color] += pColor;
}


