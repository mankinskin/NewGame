#include "../Global/stdafx.h"
#include "GUI.h"
#include "Window.h"
#include "Line.h"
#include <algorithm>
std::vector<gl::GUI::Group> gl::GUI::allGroups;
std::vector<glm::vec2> gl::GUI::allGroupDeltas;
std::vector<unsigned int> gl::GUI::allGroupQuadIndices;
std::vector<unsigned int> gl::GUI::allGroupLineIndices;


void gl::GUI::moveGroup(unsigned int pGroupIndex, glm::vec2 pDir)
{
	Group& group = allGroups[pGroupIndex];
	glm::vec4& g_quad = allQuads[group.group_quad];

	pDir = glm::vec2(std::max(std::min(pDir.x, 1.0f - (g_quad.x + g_quad.z)), -1.0f - g_quad.x ), 
			std::max(std::min(pDir.y, 1.0f - g_quad.y),  -1.0f - (g_quad.y-g_quad.w) ));
	allGroupDeltas[pGroupIndex] = glm::vec2(pDir.x, pDir.y);
}

unsigned int gl::GUI::createGroup(unsigned int pGroupQuad)
{
	allGroups.push_back(Group(pGroupQuad));
	allGroupDeltas.push_back(glm::vec2());
	return allGroups.size() - 1;
}

void gl::GUI::addQuadsToGroup(unsigned int pGroupIndex, std::initializer_list<unsigned int> pQuadIndices)
{
	Group& group = allGroups[pGroupIndex];
	group.quadOffset = group.quadCount ? group.quadOffset : allGroupQuadIndices.size();
	group.quadCount += pQuadIndices.size();
	allGroupQuadIndices.insert(allGroupQuadIndices.end(), pQuadIndices.begin(), pQuadIndices.end());
}

void gl::GUI::addLinesToGroup(unsigned int pGroupIndex, std::initializer_list<unsigned int> pLineIndices)
{
	Group& group = allGroups[pGroupIndex];
	group.lineOffset = group.lineCount ? group.lineOffset : allGroupLineIndices.size();
	group.lineCount += pLineIndices.size();
	allGroupLineIndices.insert(allGroupLineIndices.end(), pLineIndices.begin(), pLineIndices.end());
}

void gl::GUI::updateQuadGroupQuads()
{
	for (unsigned int g = 0; g < allGroups.size(); ++g) {
		Group& group = allGroups[g];
		glm::vec2& dir = allGroupDeltas[g];
		allQuads[group.group_quad] += glm::vec4(dir, 0.0f, 0.0);
		for (unsigned int q = 0; q < group.quadCount; ++q) {
			allQuads[allGroupQuadIndices[group.quadOffset + q]] += glm::vec4(dir.x, dir.y, 0.0f, 0.0f);
		}
		for (unsigned int l = 0; l < group.lineCount; ++l) {
			allLines[allGroupLineIndices[group.lineOffset + l]] += glm::vec4(dir.x, dir.y, dir.x, dir.y);
		}
		dir = glm::vec2(0.0f, 0.0f);
	}
}
