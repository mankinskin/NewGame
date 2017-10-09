#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
namespace gl{
        namespace GUI{
                void colorQuad(unsigned int pQuad, unsigned int pColor);
                void initColoredQuadVAO();
                void renderColoredQuads();
                void initColoredQuadShader();
                void updateColoredQuads();
		void setupColoredQuadShader();
        }
}
