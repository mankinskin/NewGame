#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "Quad.h"
#include "../../BaseGL/Shader/Shader.h"
#include "..\..\BaseGL\VAO.h"
#include <algorithm>
#include <App\Input\Input.h>
#include <App\Input\Mouse.h>

std::vector<gl::GUI::Quad> gl::GUI::allQuads;
size_t gl::GUI::MAX_QUAD_COUNT = 10000;
size_t gl::GUI::quadBuffer;


size_t gl::GUI::createQuad(glm::vec4 pQuad)
{
	return createQuad(Quad(pQuad));
}
size_t gl::GUI::createQuad(glm::vec2 pPos, glm::vec2 pSize)
{

	allQuads.push_back(Quad(pPos, pSize));
	return allQuads.size() - 1;

}
size_t gl::GUI::createQuad(Quad& pQuad)
{
	
	allQuads.push_back(pQuad);
	return allQuads.size() - 1;

}
size_t gl::GUI::createQuad(float pPosX, float pPosY, float pWidth, float pHeight)
{
	return createQuad(glm::vec4(pPosX, pPosY, pWidth, pHeight));
}
void gl::GUI::updateQuadBuffer()
{
	if (allQuads.size()) {
		VAO::streamStorage(quadBuffer, sizeof(Quad)*allQuads.size(), &allQuads[0]);
	}
}

void gl::GUI::reserveQuadSpace(size_t pCount)
{
	allQuads.reserve(allQuads.size() + pCount);
}

void gl::GUI::initQuadBuffer()
{
	quadBuffer = VAO::createStorage(MAX_QUAD_COUNT * sizeof(Quad), 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(quadBuffer, GL_MAP_WRITE_BIT);
	VAO::bindStorage(GL_UNIFORM_BUFFER, quadBuffer);
}

void gl::GUI::clearBuffers()
{
	allQuads.clear();
}
gl::GUI::Quad& gl::GUI::getQuad(size_t pID)
{
	return allQuads[pID];
}