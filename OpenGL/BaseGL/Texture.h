#pragma once
#include "..\Global\gl.h"
#include <unordered_map>
namespace gl {
	namespace Texture {

struct Texture2D {
	Texture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType);
	unsigned int ID;
	unsigned int width = 0;
	unsigned int height = 0;
	GLenum internalFormat;
	GLenum format;
	GLenum type;
};

struct TexData2D{
	std::vector<unsigned char> data;
	unsigned int width;
	unsigned int height;
};
extern vector<Texture2D> all2DTextures;
unsigned int createTexture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType, const void* pData);
void setTextureWrapping(unsigned int pTextureIndex, unsigned int pWrapS, unsigned int pWrapT);
void setTextureFilter(unsigned int pTextureIndex, unsigned int pMagFilter, unsigned int pMinFilter);

void setTextureDirectory(std::string pDirectory);
void resetTextureDirectory();
extern std::string TEXTURE_DIR;
extern std::unordered_map<std::string, Index> textureLookup;
	}
}