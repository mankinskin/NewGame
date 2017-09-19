#include "../Global/stdafx.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "../Global/gl.h"
#include <App\Global\Debug.h>

unsigned int gl::Texture::gBuffer;
unsigned int gl::Texture::gPosTexture;
unsigned int gl::Texture::gDepthTexture;
unsigned int gl::Texture::gNormalTexture;
unsigned int gl::Texture::gAlbedoTexture;
unsigned int gl::Texture::gAmbientTexture;
unsigned int gl::Texture::gDiffuseTexture;
unsigned int gl::Texture::gSpecularTexture;

void gl::Texture::initGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	
	gAlbedoTexture = createTexture2D(screenWidth, screenHeight, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	gPosTexture = createTexture2D(screenWidth, screenHeight, GL_RGB32F, GL_RGB, GL_FLOAT, 0);
	gDepthTexture = createTexture2D(screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	gNormalTexture = createTexture2D(screenWidth, screenHeight, GL_RGB32F, GL_RGB, GL_FLOAT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPosTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormalTexture, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthTexture, 0);

	std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(attachments.size(), &attachments[0]);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		App::Debug::pushError("Framebuffer incomplete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}
