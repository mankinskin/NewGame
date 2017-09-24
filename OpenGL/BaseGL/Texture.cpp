#include "..\Global\stdafx.h"
#include "Texture.h"
#include "..\Global\glDebug.h"
#include <App\Global\Debug.h>
#include <deps/include/SOIL/SOIL.h>
#define DEFAULT_TEXTURE_DIRECTORY "..//assets//textures//"
std::vector<gl::Texture::Texture2D> gl::Texture::all2DTextures;
std::unordered_map<std::string, gl::Index> gl::Texture::textureLookup;
std::string gl::Texture::TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;



gl::Texture::Texture2D::Texture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType) {
	width = pWidth;
	height = pHeight;
	internalFormat = pInternalFormat;
	format = pFormat;
	type = pType;
}


void gl::Texture::loadTextureBuffer(TextureBuffer & pBuffer, std::string pFilename, int pForceChannels)
{
        pBuffer.data = SOIL_load_image((TEXTURE_DIR + pFilename).c_str(), &pBuffer.width, &pBuffer.height, &pBuffer.channels, pForceChannels);
        if (!pBuffer.data) {
                App::Debug::pushError("Texture not found: \"" + (TEXTURE_DIR + pFilename) + "\" !!!");
        }

}

unsigned int gl::Texture::createTexture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType, const void* pData)
{
	Texture2D texture(pWidth, pHeight,  pInternalFormat, pFormat, pType);
	
	glGenTextures(1, &texture.ID);
	glBindTexture(GL_TEXTURE_2D, texture.ID);


	glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, texture.width, texture.height, 0, texture.format, texture.type, pData);
        glTextureParameteri(texture.ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTextureParameteri(texture.ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	all2DTextures.push_back(texture);
	Debug::getGLError("create2DTexture():");
	return texture.ID;
}

unsigned int gl::Texture::createTexture2D(std::string pFilename) 
{
        TextureBuffer buffer;
        loadTextureBuffer(buffer, pFilename);
        return createTexture2D(buffer);
}
unsigned int gl::Texture::createTexture2D(TextureBuffer pBuffer)
{
        int internal_format = 0;
        int format = 0;
        switch (pBuffer.channels) {
        case 4:
                internal_format = GL_RGBA8;
                format = GL_RGBA;
                break;
        case 3:
                internal_format = GL_RGB8;
                format = GL_RGB;
                break;
        case 1:
                internal_format = GL_R8;
                format = GL_RED;
                break;
        case 2:
                internal_format = GL_RG8;
                format = GL_RG;
                break;
        default:
                internal_format = GL_RGBA8;
                format = GL_RGBA;
        }
        return createTexture2D(pBuffer.width, pBuffer.height, internal_format, format, GL_UNSIGNED_BYTE, pBuffer.data);
}

void gl::Texture::setTextureWrapping(unsigned int pTextureIndex, unsigned int pWrapS, unsigned int pWrapT)
{
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_S, pWrapS);
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_T, pWrapT);
}

void gl::Texture::setTextureFilter(unsigned int pTextureIndex, unsigned int pMagFilter, unsigned int pMinFilter)
{
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_MIN_FILTER, pMinFilter);
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_MAG_FILTER, pMagFilter);
}

void gl::Texture::setTextureDirectory(std::string pDirectory)
{
	TEXTURE_DIR = pDirectory;
}

void gl::Texture::resetTextureDirectory()
{
	TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;
}
