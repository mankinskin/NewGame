#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "GUI.h"
#include "..\..\Global\glDebug.h"
#include <App\Global\Debug.h>
#include "..\..\BaseGL\VAO.h"
#include "Quad.h"
#include "Line.h"

std::unordered_map<std::string, size_t> gl::GUI::colorLookup;
std::vector<glm::vec4> gl::GUI::allColors;
size_t gl::GUI::colorBuffer;
size_t SCREEN_QUAD = 0;

void gl::GUI::init()
{
	SCREEN_QUAD = createQuad(-1.0f, 1.0f, 2.0f, 2.0f);
}
size_t gl::GUI::createColor(glm::vec4 pColor, std::string pColorName)
{
	size_t i = allColors.size();
	allColors.push_back(pColor);
	if (pColorName.size()) {
		colorLookup.insert(std::pair<std::string, size_t>(pColorName, i));
	}
	return i;
}
void gl::GUI::storeGUIColors()
{
	colorBuffer = VAO::createStorage(sizeof(glm::vec4)*allColors.size(), &allColors[0], 0);
	VAO::bindStorage(GL_UNIFORM_BUFFER, colorBuffer);
}