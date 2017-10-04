#include "../Global/stdafx.h"
#include "Buttons.h"
#include "../Global/glDebug.h"
#include "../BaseGL/Framebuffer.h"
#include "../BaseGL/Shader.h"
#include "../BaseGL/VAO.h"
#include <App/Input/Mouse.h>

unsigned int gl::GUI::buttonVAO;
unsigned int gl::GUI::buttonBuffer;
unsigned int gl::GUI::buttonIndexShader;
std::vector<int> gl::GUI::allButtonFlags;
std::vector<unsigned int> gl::GUI::allButtonQuads;
std::vector<unsigned int> gl::GUI::buttonIndexMap;


void gl::GUI::rasterButtons() {
        if (allButtonQuads.size()) {
                glBindFramebuffer(GL_FRAMEBUFFER, gl::Texture::guiFBO);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDepthMask(0);
                glBindVertexArray(buttonVAO);
                gl::Shader::use(buttonIndexShader);
                
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allButtonQuads.size());
                
                gl::Shader::unuse();
                glBindVertexArray(0);
                glDepthMask(1);
                Debug::getGLError("rasterButtons()");
        }
}
void gl::GUI::initButtonIndexShader() {
        buttonIndexShader = gl::Shader::newProgram("buttonIndexShader", gl::Shader::createModule("buttonIndexShader.vert"), gl::Shader::createModule("buttonIndexShader.frag"));
        gl::Shader::addVertexAttribute(buttonIndexShader, "corner_pos", 0);
        gl::Shader::addVertexAttribute(buttonIndexShader, "quad_index", 1);
}

void gl::GUI::initButtonBuffer()
{
        buttonBuffer = gl::VAO::createStorage(sizeof(unsigned int) * 10000, 0, GL_MAP_WRITE_BIT | gl::VAO::STREAM_FLAGS);
        gl::VAO::createStream(buttonBuffer, GL_MAP_WRITE_BIT);

        glCreateVertexArrays(1, &buttonVAO);
        glVertexArrayElementBuffer(buttonVAO, gl::quadEBO+1);
        glVertexArrayVertexBuffer(buttonVAO, 0, gl::quadVBO+1, 0, sizeof(glm::vec2));
        gl::VAO::setVertexAttrib(buttonVAO, 0, 0, 2, GL_FLOAT, 0);

        gl::VAO::setVertexArrayVertexStorage(buttonVAO, 1, buttonBuffer, sizeof(unsigned int));
        gl::VAO::setVertexAttrib(buttonVAO, 1, 1, 1, GL_UNSIGNED_INT, 0);
        glVertexArrayBindingDivisor(buttonVAO, 1, 1);

        buttonIndexMap.resize(gl::screenWidth * gl::screenHeight);
}

void gl::GUI::updateButtonBuffer()
{
        if (allButtonQuads.size()) {
                gl::VAO::streamStorage(buttonBuffer, sizeof(unsigned int)*allButtonQuads.size(), &allButtonQuads[0]);
        }
}

unsigned int gl::GUI::addButton(unsigned int pQuadIndex)
{
        allButtonQuads.push_back(pQuadIndex);
        return allButtonQuads.size();
}
void gl::GUI::toggleButton(unsigned int pButtonID) {
        allButtonFlags[pButtonID - 1] = !allButtonFlags[pButtonID - 1];
}
void gl::GUI::hideButton(unsigned int pButtonID) {
        allButtonFlags[pButtonID - 1] = 0;
}
void gl::GUI::unhideButton(unsigned int pButtonID) {
        allButtonFlags[pButtonID - 1] = 1;
}
void gl::GUI::fetchButtonMap()
{
        //get button id pixels
        glGetTextureImage(gl::Texture::buttonIndexTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, gl::screenWidth * gl::screenHeight, &buttonIndexMap[0]);
}
void gl::GUI::clearButtons()
{
        allButtonFlags.clear();
        allButtonQuads.clear();
}