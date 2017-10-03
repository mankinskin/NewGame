#include "../Global/stdafx.h"
#include "GUI.h"
#include "Group.h"
#include "Line.h"
#include <algorithm>
std::vector<unsigned int> gl::GUI::allGroupBounds;
std::vector<glm::vec2> gl::GUI::allGroupDeltas;
std::vector<gl::GUI::QuadElement> gl::GUI::allQuadElements;
std::vector<gl::GUI::LineElement> gl::GUI::allLineElements;


void gl::GUI::moveGroup(unsigned int pGroupIndex, glm::vec2 pDir)
{
	glm::vec4 g_quad = allQuads[allGroupBounds[pGroupIndex]];
	pDir = glm::vec2(std::max(std::min(pDir.x, 1.0f - (g_quad.x + g_quad.z)), -1.0f - g_quad.x ), 
			std::max(std::min(pDir.y, 1.0f - g_quad.y),  -1.0f - (g_quad.y-g_quad.w) ));
	allGroupDeltas[pGroupIndex] = glm::vec2(pDir.x, pDir.y);
}

unsigned int gl::GUI::createGroup(unsigned int pGroupQuad)
{
	allGroupBounds.push_back(pGroupQuad);
	allGroupDeltas.push_back(glm::vec2());
	return allGroupDeltas.size() - 1;
}

void gl::GUI::addQuadToGroup(unsigned int pQuadIndex, unsigned int pGroupIndex)
{
	allQuadElements.push_back(QuadElement(pQuadIndex, pGroupIndex));
}

void gl::GUI::addLineToGroup(unsigned int pLineIndex, unsigned int pGroupIndex)
{
	allLineElements.push_back(LineElement(pLineIndex, pGroupIndex));
}

void gl::GUI::updateGroups()
{
	for (unsigned int g = 0; g < allGroupBounds.size(); ++g) {
		glm::vec4& g_quad = allQuads[allGroupBounds[g]];
		glm::vec2& delta = allGroupDeltas[g];
		delta = glm::vec2(std::max(std::min(delta.x, 1.0f - (g_quad.x + g_quad.z)), -1.0f - g_quad.x),
			std::max(std::min(delta.y, 1.0f - g_quad.y), -1.0f - (g_quad.y - g_quad.w)));
	}
	for (unsigned int qe = 0; qe < allQuadElements.size(); ++qe) {
		QuadElement& elem = allQuadElements[qe];
		glm::vec2& dir = allGroupDeltas[elem.parent_group];
		allQuads[elem.quad_index] += glm::vec4(dir.x, dir.y, 0.0f, 0.0f);
	}

	for (unsigned int le = 0; le < allLineElements.size(); ++le) {
		LineElement& elem = allLineElements[le];
		glm::vec2& dir = allGroupDeltas[elem.parent_group];
		allLines[elem.line_index] += glm::vec4(dir.x, dir.y, dir.x, dir.y);
	}

	std::fill(allGroupDeltas.begin(), allGroupDeltas.end(), glm::vec2());
}
