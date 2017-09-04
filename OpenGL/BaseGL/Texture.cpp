#include "..\Global\stdafx.h"
#include "Texture.h"
#include "..\Global\glDebug.h"
#define DEFAULT_TEXTURE_DIRECTORY "..//deps//textures//"
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


unsigned int gl::Texture::createTexture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType, const void* pData)
{
	Texture2D texture(pWidth, pHeight,  pInternalFormat, pFormat, pType);
	
	glGenTextures(1, &texture.ID);
	glBindTexture(GL_TEXTURE_2D, texture.ID);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, texture.width, texture.height, 0, texture.format, texture.type, pData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	all2DTextures.push_back(texture);
	Debug::getGLError("create2DTexture():");
	return all2DTextures.size() - 1;
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
