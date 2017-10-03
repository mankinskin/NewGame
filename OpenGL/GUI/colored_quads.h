#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
namespace gl{
        namespace GUI{
                struct Colored_Quad{
                        Colored_Quad(unsigned pQuad, unsigned pColor)
                        :quad(pQuad), color(pColor){}
                        unsigned int quad;
                        unsigned int color;
                };


                
                
                void colorQuad(unsigned int pQuad, unsigned int pColor);
                void initColoredQuadVAO();
                void renderColoredQuads();
                void initColoredQuadShader();
                void updateColoredQuads();
                
                extern unsigned int coloredQuadShader;
                extern unsigned int coloredQuadBuffer;
                
                extern unsigned int coloredQuadVAO;
                extern std::vector<Colored_Quad> allColoredQuads;
                
        }
}
