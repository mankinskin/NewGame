#include "..\Global\stdafx.h"
#include "GUI.h"
#include "..\BaseGL\Shader.h"
#include "..\BaseGL\VAO.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>
#include <App\Input\Input.h>
#include <App\Input\Mouse.h>
#include "Window.h"
std::vector<glm::vec4> gl::GUI::allQuads;
std::vector<int> gl::GUI::allQuadFlags;
unsigned int gl::GUI::MAX_QUAD_COUNT = 10000;
unsigned int gl::GUI::quadBuffer;
unsigned int gl::GUI::colorBuffer;
std::unordered_map<std::string, unsigned int> gl::GUI::colorLookup;

unsigned int gl::GUI::createColor(glm::vec4 pColor, std::string pColorName) {
	unsigned int i = allColors.size();
	allColors.push_back(pColor);
	if (pColorName.size()) {
		colorLookup.insert(std::pair<std::string, unsigned int>(pColorName, i));
	}
	return i;
}

void gl::GUI::storeGUIColors()
{
	colorBuffer = VAO::createStorage(sizeof(glm::vec4)*allColors.size(), &allColors[0], 0);
	VAO::bindStorage(GL_UNIFORM_BUFFER, colorBuffer);
}

void gl::GUI::clearBuffers()
{
	allQuads.clear();
        allQuadFlags.clear();
}

void gl::GUI::moveQuadByMouseDelta(unsigned int pQuadIndex)
{
        allQuads[pQuadIndex].x = glm::clamp(allQuads[pQuadIndex].x + App::Input::cursorFrameDelta.x, -1.0, 1.0 - allQuads[pQuadIndex].z);
        allQuads[pQuadIndex].y = glm::clamp(allQuads[pQuadIndex].y + App::Input::cursorFrameDelta.y, -1.0 + allQuads[pQuadIndex].w, 1.0);
}

void gl::GUI::moveQuadGroupByMouseDelta(unsigned int GroupIndex)
{
	moveGroup(GroupIndex, App::Input::cursorFrameDelta);
}

void gl::GUI::setQuadVisibility(unsigned int pQuadIndex, int pHide) {
	allQuadFlags[pQuadIndex] = pHide;
}

unsigned gl::GUI::createQuad(glm::vec4 pQuad)
{
        allQuads.push_back(pQuad);
        return allQuads.size()-1;
}
unsigned gl::GUI::createQuad(glm::vec2 pPos, glm::vec2 pSize)
{
	return createQuad(glm::vec4(pPos, pSize));
	
}
unsigned gl::GUI::createQuad(float pPosX, float pPosY, float pWidth, float pHeight)
{
	return createQuad(glm::vec4(pPosX, pPosY, pWidth, pHeight));
}

void gl::GUI::reserveQuadSpace(unsigned int pCount)
{
	allQuads.reserve(pCount);
	allQuadFlags.reserve(pCount);
}

void gl::GUI::initQuadBuffer()
{
        quadBuffer = VAO::createStorage(MAX_QUAD_COUNT * sizeof(glm::vec4), 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
        VAO::createStream(quadBuffer, GL_MAP_WRITE_BIT);
        VAO::bindStorage(GL_UNIFORM_BUFFER, quadBuffer);

}

void gl::GUI::updateQuadBuffer() 
{
	if (allQuads.size()) {
	        VAO::streamStorage(quadBuffer, sizeof(glm::vec4)*allQuads.size(), &allQuads[0]);
        }
}