#include "..\Global\stdafx.h"
#include "GUI.h"
#include "..\BaseGL\Shader.h"
#include "..\BaseGL\Shader_Data.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>

std::vector<glm::vec2> gl::GUI::allPositions;
std::vector<glm::vec2> gl::GUI::allSizes;
std::vector<glm::vec4> gl::GUI::allColors;
std::vector<glm::vec4> gl::GUI::allUVRanges;

std::vector<gl::GUI::QuadElement> gl::GUI::allQuads;

unsigned int gl::GUI::MAX_GUI_QUADS = 10000;

unsigned int gl::GUI::guiVAO = 0;

unsigned int gl::GUI::guiQuadBuffer;
unsigned int gl::GUI::guiColorBuffer = 0;
unsigned int gl::GUI::guiPositionBuffer = 0;
unsigned int gl::GUI::guiSizeBuffer = 0;
unsigned int gl::GUI::guiUVBuffer = 0;

unsigned int gl::GUI::guiTexQuadShader = 0;
unsigned int gl::GUI::guiQuadShader = 0;


void gl::GUI::createQuads()
{

	allPositions.reserve(2);
	Pos p1 = gl::GUI::createPos(-0.99, -0.80);
	Pos p2 = gl::GUI::createPos(-0.99,- 0.90);

	allSizes.reserve(2);
	Size s1 = gl::GUI::createSize(0.3, 0.07);
	Size s2 = gl::GUI::createSize(0.3, 0.07);


	allColors.reserve(2);
	Color c1 = gl::GUI::createColor(0.95f, 0.95f, 0.9f, 1.0f);
	Color c2 = gl::GUI::createColor(0.95f, 0.95f, 0.9f, 1.0f);


	allQuads.reserve(2);
	createQuad(p1, s1, c1);
	createQuad(p2, s2, c2);
}

void gl::GUI::clearBuffers()
{
	allQuads.clear();
	allPositions.clear();
	allSizes.clear();
	allColors.clear();
}


gl::GUI::Quad gl::GUI::createQuad(Pos pPos, Size pSize, Color pColor, UV pUVRange)
{
	allQuads.push_back(QuadElement(pPos, pSize, pColor, pUVRange));
	return allQuads.size() - 1;
}

void gl::GUI::initGUIBuffers()
{
	//BUFFERS
	
	guiPositionBuffer = Shader::Data::createStorage( MAX_GUI_QUADS*sizeof(float)*2, nullptr, GL_MAP_WRITE_BIT | Shader::Data::STREAM_FLAGS);
	Shader::Data::createStream(guiPositionBuffer, GL_MAP_WRITE_BIT);
	Shader::Data::bindStorage(GL_UNIFORM_BUFFER, guiPositionBuffer);

	guiSizeBuffer = Shader::Data::createStorage(MAX_GUI_QUADS * sizeof(float) * 2, nullptr, GL_MAP_WRITE_BIT | Shader::Data::STREAM_FLAGS);
	Shader::Data::createStream(guiSizeBuffer, GL_MAP_WRITE_BIT);
	Shader::Data::bindStorage(GL_UNIFORM_BUFFER, guiSizeBuffer);

	guiColorBuffer = Shader::Data::createStorage(MAX_GUI_QUADS * sizeof(float) * 4, nullptr, GL_MAP_WRITE_BIT | Shader::Data::STREAM_FLAGS);
	Shader::Data::createStream(guiColorBuffer, GL_MAP_WRITE_BIT);
	Shader::Data::bindStorage(GL_UNIFORM_BUFFER, guiColorBuffer);

	guiUVBuffer = Shader::Data::createStorage(MAX_GUI_QUADS * sizeof(float) * 4, nullptr, GL_MAP_WRITE_BIT | Shader::Data::STREAM_FLAGS);
	Shader::Data::createStream(guiUVBuffer, GL_MAP_WRITE_BIT);
	Shader::Data::bindStorage(GL_UNIFORM_BUFFER, guiUVBuffer);

	

	//VAO
	glCreateVertexArrays(1, &guiVAO);

	//basic, per instance, quad mesh
	glVertexArrayVertexBuffer(guiVAO, 0, quadVBO, 0, sizeof(glm::vec2));
	glVertexArrayElementBuffer(guiVAO, quadEBO);

	//quad indices
	guiQuadBuffer = Shader::Data::createStorage(MAX_GUI_QUADS * sizeof(glm::ivec4), nullptr, GL_MAP_WRITE_BIT | Shader::Data::STREAM_FLAGS);
	Shader::Data::createStream(guiQuadBuffer, GL_MAP_WRITE_BIT);
	Shader::Data::bindVertexArrayVertexStorage(guiQuadBuffer, guiVAO, GL_ARRAY_BUFFER, 1, sizeof(glm::ivec4));
	glVertexArrayBindingDivisor(guiVAO, 1, 1);

	Shader::Data::initVertexAttrib(guiVAO, 0, 0, 2, GL_FLOAT, 0);

	Shader::Data::initVertexAttrib(guiVAO, 1, 1, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, x));
	Shader::Data::initVertexAttrib(guiVAO, 1, 2, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, y));
	Shader::Data::initVertexAttrib(guiVAO, 1, 3, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, z));
	Shader::Data::initVertexAttrib(guiVAO, 1, 4, 1, GL_UNSIGNED_INT, offsetof(glm::ivec4, w));
	
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
		QuadElement& quad = allQuads[q];
		*(it++) = glm::ivec4(
			&allPositions[quad.pos] - &allPositions[0], &allSizes[quad.size] - &allSizes[0],
			&allColors[quad.color] - &allColors[0], 0);
	}


	Shader::Data::streamStorage(guiQuadBuffer, sizeof(glm::ivec4)*indexQuads.size(), &indexQuads[0]);
	Shader::Data::streamStorage(guiPositionBuffer, sizeof(glm::vec2)*allPositions.size(), &allPositions[0]);
	Shader::Data::streamStorage(guiSizeBuffer, sizeof(glm::vec2)*allSizes.size(), &allSizes[0]);
	Shader::Data::streamStorage(guiColorBuffer, sizeof(glm::vec4)*allColors.size(), &allColors[0]);
	//Shader::Data::streamTargetStorage(guiUVBuffer, sizeof(glm::vec4)*allUVRanges.size(), &allUVRanges[0]);
}

void gl::GUI::initGUIShaders()
{
	
	guiQuadShader = Shader::newProgram("guiQuad", Shader::newModule("guiQuad.vert"), Shader::newModule("guiQuad.frag")).ID;
	Shader::Data::addVertexAttribute(guiQuadShader, "posIndex", 0);
	Shader::Data::addVertexAttribute(guiQuadShader, "sizeIndex", 1);
	Shader::Data::addVertexAttribute(guiQuadShader, "uvIndex", 2);
	Shader::Data::addVertexAttribute(guiQuadShader, "colorIndex", 3);
}

void gl::GUI::renderGUI()
{
	glBindVertexArray(guiVAO);

	Shader::use(guiQuadShader);
	
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allQuads.size());
	
	Shader::unuse();
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


