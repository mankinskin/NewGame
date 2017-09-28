#include "../Global/stdafx.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "../Global/gl.h"
#include <App\Global\Debug.h>

unsigned int gl::Texture::gBuffer;
unsigned int gl::Texture::gPosTexture;
unsigned int gl::Texture::gDepthTexture;
unsigned int gl::Texture::gNormalTexture;
unsigned int gl::Texture::gAmbientTexture;
unsigned int gl::Texture::gDiffuseTexture;
unsigned int gl::Texture::gSpecularTexture;

unsigned int gl::Texture::lightFBO;
unsigned int gl::Texture::lightColorTexture;
unsigned int gl::Texture::lightDepthTexture;

unsigned int gl::Texture::buttonFBO;
unsigned int gl::Texture::buttonIndexTexture;
unsigned int gl::Texture::buttonDepthTexture;

void gl::Texture::initGBuffer()
{
	glCreateFramebuffers(1, &gBuffer);
	
        gAmbientTexture = createTexture2D(screenWidth, screenHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
        gDiffuseTexture = createTexture2D(screenWidth, screenHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
        gSpecularTexture = createTexture2D(screenWidth, screenHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
	gPosTexture = createTexture2D(screenWidth, screenHeight, GL_RGB32F, GL_RGB, GL_FLOAT, 0);
	gDepthTexture = createTexture2D(screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	gNormalTexture = createTexture2D(screenWidth, screenHeight, GL_RGB32F, GL_RGB, GL_FLOAT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAmbientTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gDiffuseTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gSpecularTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gPosTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gNormalTexture, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthTexture, 0);

        unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, &attachments[0]);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		App::Debug::pushError("Framebuffer incomplete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}
void gl::Texture::initLightFBO(){
	glCreateFramebuffers(1, &lightFBO);
	lightColorTexture = createTexture2D(screenWidth, screenHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
	lightDepthTexture = createTexture2D(screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightColorTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lightDepthTexture, 0);
	
        unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, &attachments[0]);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		App::Debug::pushError("Framebuffer incomplete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void gl::Texture::initButtonFBO(){
        glCreateFramebuffers(1, & buttonFBO);
	buttonIndexTexture = createTexture2D(screenWidth, screenHeight, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
	buttonDepthTexture = createTexture2D(screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, buttonFBO);
        
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buttonIndexTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buttonDepthTexture, 0);
	
        unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, &attachments[0]);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		App::Debug::pushError("Framebuffer incomplete");
	}
        
        glClearColor(0, 0, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
