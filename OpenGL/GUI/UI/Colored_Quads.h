#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
namespace gl {
    namespace GUI {
	void colorQuad(size_t pQuad, size_t pColor);
	void initColoredQuadVAO();
	void renderColoredQuads();
	void initColoredQuadShader();
	void updateColoredQuads();
	void setupColoredQuadShader();
    }
}
