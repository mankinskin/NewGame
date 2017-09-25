#include <unordered_map>
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


                unsigned int createColor(glm::vec4 pColor, std::string pColorName = "");
                void storeGUIColors();
                
                void colorQuad(unsigned int pQuad, unsigned int pColor);
                void initColoredQuadVAO();
                void renderColoredQuads();
                void initColoredQuadShader();
                void updateColoredQuads();
                
                extern unsigned int coloredQuadShader;
                extern unsigned int coloredQuadBuffer;
                extern unsigned int quadColorBuffer;
                extern unsigned int coloredQuadVAO;
                extern std::vector<Colored_Quad> allColoredQuads;
                extern std::vector<glm::vec4> allColors;
                extern std::unordered_map<std::string, unsigned int> colorLookup;
        }
}
