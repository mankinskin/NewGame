#include "../Global/stdafx.h"
#include "Buttons.h"
#include "../Global/glDebug.h"
#include "../BaseGL/Framebuffer.h"
#include "../BaseGL/Shader.h"
#include "../BaseGL/VAO.h"
#include <App/Input/Buttons.h>

unsigned int gl::GUI::buttonVAO;
unsigned int gl::GUI::buttonBuffer;
unsigned int gl::GUI::buttonIndexShader;

void gl::GUI::rasterButtons() {
        Debug::getGLError("rasterButtons()1");
        if (App::Input::allButtonQuads.size()) {
                glBindFramebuffer(GL_FRAMEBUFFER, gl::Texture::buttonFBO);
                glBindVertexArray(buttonVAO);
                gl::Shader::use(buttonIndexShader);

                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, App::Input::allButtonQuads.size());
                
                gl::Shader::unuse();
                glBindVertexArray(0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                Debug::getGLError("rasterButtons()");
        }
}
void gl::GUI::initButtonIndexShader() {
        buttonIndexShader = gl::Shader::newProgram("buttonIndexShader", gl::Shader::newModule("buttonIndexShader.vert"), gl::Shader::newModule("buttonIndexShader.frag")).ID;
        gl::Shader::addVertexAttribute(buttonIndexShader, "corner_pos", 0);
        gl::Shader::addVertexAttribute(buttonIndexShader, "quad_index", 1);
}

void gl::GUI::initButtonBuffer()
{
        buttonBuffer = gl::VAO::createStorage(sizeof(unsigned int) * 100, 0, GL_MAP_WRITE_BIT | gl::VAO::STREAM_FLAGS);
        gl::VAO::createStream(buttonBuffer, GL_MAP_WRITE_BIT);

        glCreateVertexArrays(1, &buttonVAO);
        glVertexArrayElementBuffer(buttonVAO, gl::quadEBO);
        glVertexArrayVertexBuffer(buttonVAO, 0, gl::quadVBO, 0, sizeof(glm::vec2));
        gl::VAO::setVertexAttrib(buttonVAO, 0, 0, 2, GL_FLOAT, 0);

        gl::VAO::setVertexArrayVertexStorage(buttonVAO, 1, buttonBuffer, sizeof(unsigned int));
        gl::VAO::setVertexAttrib(buttonVAO, 1, 1, 1, GL_UNSIGNED_INT, sizeof(unsigned int));
        glVertexArrayBindingDivisor(buttonVAO, 1, 1);
}

void gl::GUI::updateButtonBuffer()
{
        if (App::Input::allButtonQuads.size()) {
                gl::VAO::streamStorage(buttonBuffer, sizeof(unsigned int)*App::Input::allButtonQuads.size(), &App::Input::allButtonQuads[0]);
        }
}