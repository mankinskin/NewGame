#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "GUI.h"
#include "../../BaseGL/Shader/Shader.h"
#include "..\..\BaseGL\VAO.h"
#include "..\..\Global\glDebug.h"
#include <App\Global\Debug.h>
#include <App\Input\Input.h>
#include <App\Input\Mouse.h>
#include <algorithm>

std::vector<glm::vec4> gl::GUI::allColors;
std::vector<glm::vec4> gl::GUI::allQuads;
std::vector<glm::vec2> gl::GUI::allQuadDeltas;
std::vector<int> gl::GUI::allQuadFlags;
unsigned int gl::GUI::MAX_QUAD_COUNT = 10000;
unsigned int gl::GUI::quadBuffer;
unsigned int gl::GUI::colorBuffer;
std::unordered_map<std::string, unsigned int> gl::GUI::colorLookup;
std::vector<unsigned int> allQuadParents;


unsigned gl::GUI::createQuad(glm::vec4 pQuad)
{
        allQuads.push_back(pQuad);
	allQuadDeltas.push_back(glm::vec2());
	allQuadFlags.push_back(0);
	allQuadParents.push_back(0);
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

void gl::GUI::bindQuadParent(unsigned int pParentQuad, unsigned int pChildQuad) 
{
	allQuadParents[pChildQuad] = pParentQuad + 1;
}

void gl::GUI::moveQuadByMouseDelta(unsigned int pQuadIndex)
{
	moveQuad(pQuadIndex, App::Input::cursorFrameDelta);
}

void gl::GUI::moveQuad(unsigned int pQuadIndex, glm::vec2 pDelta) 
{
	allQuadDeltas[pQuadIndex] = pDelta;
	//allQuads[pQuadIndex].x = glm::clamp(allQuads[pQuadIndex].x + pDelta.x, -1.0f, 1.0f - allQuads[pQuadIndex].z);
	//allQuads[pQuadIndex].y = glm::clamp(allQuads[pQuadIndex].y + pDelta.y, -1.0f + allQuads[pQuadIndex].w, 1.0f);
}

void gl::GUI::updateElements()
{
	//update deltas
	for (unsigned int q = 0; q < allQuads.size(); ++q) {
		glm::vec2& delta = allQuadDeltas[q];
		unsigned int parent = allQuadParents[q];//parents are offset by one when stored, so that there cann be null parent
		delta += parent > 0 ? allQuadDeltas[parent - 1] : glm::vec2();
	}
	for (unsigned int q = 0; q < allQuads.size(); ++q) {
		allQuads[q] += glm::vec4(allQuadDeltas[q], 0.0f, 0.0f);
	}
	std::fill(allQuadDeltas.begin(), allQuadDeltas.end(), glm::vec2());
}
void gl::GUI::updateQuadBuffer() 
{
	if (allQuads.size()) {
	        VAO::streamStorage(quadBuffer, sizeof(glm::vec4)*allQuads.size(), &allQuads[0]);
        }
}

void gl::GUI::reserveQuadSpace(unsigned int pCount)
{
	allQuads.reserve(allQuads.size() + pCount);
	allQuadDeltas.reserve(allQuadDeltas.size() + pCount);
	allQuadFlags.reserve(allQuadFlags.size() + pCount);
}

void gl::GUI::initQuadBuffer()
{
        quadBuffer = VAO::createStorage(MAX_QUAD_COUNT * sizeof(glm::vec4), 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
        VAO::createStream(quadBuffer, GL_MAP_WRITE_BIT);
        VAO::bindStorage(GL_UNIFORM_BUFFER, quadBuffer);

}
unsigned int gl::GUI::createColor(glm::vec4 pColor, std::string pColorName) 
{
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
glm::vec4& gl::GUI::getQuad(unsigned int pID) 
{
	return allQuads[pID];
}

