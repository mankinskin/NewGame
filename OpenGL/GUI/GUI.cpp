#include "..\Global\stdafx.h"
#include "GUI.h"
#include "..\BaseGL\Shader.h"
#include "..\BaseGL\VAO.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>

std::vector<glm::vec4> gl::GUI::allQuads;
std::vector<int> gl::GUI::allQuadFlags;
unsigned int gl::GUI::MAX_QUAD_COUNT = 10000;
unsigned int gl::GUI::quadBuffer;


void gl::GUI::clearBuffers()
{
	allQuads.clear();
        allQuadFlags.clear();
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