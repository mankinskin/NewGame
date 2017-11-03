#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "Quad.h"
#include "../../BaseGL/Shader/Shader.h"
#include "..\..\BaseGL\VAO.h"
#include <algorithm>
#include <App\Input\Input.h>
#include <App\Input\Mouse.h>

std::vector<glm::vec4> gl::GUI::allQuads;
size_t gl::GUI::MAX_QUAD_COUNT = 10000;
size_t gl::GUI::quadBuffer;


void gl::GUI::updateQuadBuffer()
{
	if (allQuads.size()) {
		VAO::streamStorage(quadBuffer, sizeof(glm::vec4)*allQuads.size(), &allQuads[0]);
	}
}

size_t gl::GUI::createQuad(float pPosX, float pPosY, float pWidth, float pHeight)
{
	allQuads.emplace_back(pPosX, pPosY, pWidth, pHeight);
	return allQuads.size();
}
void gl::GUI::reserveQuads(size_t pCount)
{
	allQuads.reserve(allQuads.size() + pCount);
}

void gl::GUI::initQuadBuffer()
{
	quadBuffer = VAO::createStorage(MAX_QUAD_COUNT * sizeof(glm::vec4), 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(quadBuffer, GL_MAP_WRITE_BIT);
	VAO::bindStorage(GL_UNIFORM_BUFFER, quadBuffer);
}
void gl::GUI::clearBuffers()
{
	allQuads.clear();
}

