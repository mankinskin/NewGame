#include "../Global/stdafx.h"
#include "GUI.h"
#include "Colored_Quads.h"
#include "../BaseGL/VAO.h"
#include "../BaseGL/Shader.h"
#include "../Global/gl.h"
std::vector<glm::vec4> gl::GUI::allColors;
std::vector<gl::GUI::Colored_Quad> gl::GUI::allColoredQuads;
std::unordered_map<std::string, unsigned int> gl::GUI::colorLookup;
unsigned int gl::GUI::coloredQuadBuffer;
unsigned int gl::GUI::quadColorBuffer;
unsigned int gl::GUI::coloredQuadVAO;
unsigned int gl::GUI::coloredQuadShader;

unsigned int gl::GUI::createColor(glm::vec4 pColor, std::string pColorName){
        unsigned int i = allColors.size();
        allColors.push_back(pColor);
        if(pColorName.size()){
                colorLookup.insert(std::pair<std::string, unsigned int>(pColorName, i));
        }
        return i;
}

void gl::GUI::storeGUIColors()
{
        quadColorBuffer = VAO::createStorage(sizeof(glm::vec4)*allColors.size(), &allColors[0], 0);
        VAO::bindStorage(quadColorBuffer, GL_UNIFORM_BUFFER);
}
void gl::GUI::colorQuad(unsigned int pQuad, unsigned int pColor){
        allColoredQuads.push_back(Colored_Quad(pQuad, pColor));
}

void gl::GUI::renderColoredQuads(){
    glBindVertexArray(coloredQuadVAO);    
    Shader::use(quadColorBuffer);
    
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allColoredQuads.size());

    Shader::unuse();
    glBindVertexArray(0);    
}
void gl::GUI::initColoredQuadVAO(){
        
        coloredQuadBuffer = VAO::createStorage(sizeof(Colored_Quad)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
        VAO::createStream(coloredQuadBuffer, GL_MAP_WRITE_BIT);

        glCreateVertexArrays(1, &coloredQuadVAO);
        glVertexArrayElementBuffer(coloredQuadVAO, quadEBO);
        glVertexArrayVertexBuffer(coloredQuadVAO, 0, quadVBO, 0, sizeof(glm::vec2));
        VAO::setVertexAttrib(coloredQuadVAO, 0, 0, 2, GL_FLOAT, 0);

        VAO::setVertexArrayVertexStorage(coloredQuadVAO, 1, coloredQuadBuffer, sizeof(Colored_Quad));
        VAO::setVertexAttrib(coloredQuadVAO, 1, 1, 2, GL_UNSIGNED_INT, sizeof(Colored_Quad));
        glVertexArrayBindingDivisor(coloredQuadVAO, 1, 1);
}



void gl::GUI::initColoredQuadShader(){
        coloredQuadShader = Shader::newProgram("coloredQuadShader", Shader::newModule("coloredQuadShader.vert"), Shader::newModule("coloredQuadShader.frag")).ID; 
        Shader::addVertexAttribute(coloredQuadShader, "corner_pos", 0);
        Shader::addVertexAttribute(coloredQuadShader, "colored_quad", 1);
}

void gl::GUI::updateColoredQuads()
{
        if (allColoredQuads.size()) {
                VAO::streamStorage(coloredQuadBuffer, sizeof(Colored_Quad)*allColoredQuads.size(), &allColoredQuads[0]);
        }
}


