#include "../Global/stdafx.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "../Global/gl.h"
#include <App\Global\Debug.h>
#include <App\Global\App.h>
size_t gl::Texture::gBuffer;
size_t gl::Texture::gPosTexture;
size_t gl::Texture::gNormalTexture;
size_t gl::Texture::gAmbientTexture;
size_t gl::Texture::gDiffuseTexture;
size_t gl::Texture::gSpecularTexture;
size_t gl::Texture::gDepthRenderbuffer;

size_t gl::Texture::guiFBO;
size_t gl::Texture::fontColorTexture;
size_t gl::Texture::buttonIndexTexture;
size_t gl::Texture::guiDepthRenderbuffer;


void gl::Texture::initGBuffer()
{
    glCreateFramebuffers(1, &gBuffer);
    glCreateRenderbuffers(1, &gDepthRenderbuffer);

    gAmbientTexture = createTexture2D(screenWidth * resolution, screenHeight * resolution, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
    gDiffuseTexture = createTexture2D(screenWidth * resolution, screenHeight * resolution, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
    gSpecularTexture = createTexture2D(screenWidth * resolution, screenHeight * resolution, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
    gPosTexture = createTexture2D(screenWidth * resolution, screenHeight * resolution, GL_RGB32F, GL_RGB, GL_FLOAT, 0);
    gNormalTexture = createTexture2D(screenWidth * resolution, screenHeight * resolution, GL_RGB32F, GL_RGB, GL_FLOAT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAmbientTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gDiffuseTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gSpecularTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gPosTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gNormalTexture, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, gDepthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth * resolution, screenHeight * resolution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthRenderbuffer);

    size_t attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glNamedFramebufferDrawBuffers(gBuffer, 5, &attachments[0]);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
	App::Debug::pushError("Framebuffer incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

}

void gl::Texture::initGUIFBO() {
    glCreateFramebuffers(1, &guiFBO);
    glCreateRenderbuffers(1, &guiDepthRenderbuffer);
    buttonIndexTexture = createTexture2D(screenWidth, screenHeight, GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
    fontColorTexture = createTexture2D(screenWidth, screenHeight, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, guiFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buttonIndexTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fontColorTexture, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, guiDepthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, guiDepthRenderbuffer);

    size_t attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glNamedFramebufferDrawBuffers(guiFBO, 2, &attachments[0]);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
	App::Debug::pushError("Framebuffer incomplete");
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}