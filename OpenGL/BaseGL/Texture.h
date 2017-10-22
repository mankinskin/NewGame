#pragma once
#include "..\Global\gl.h"
#include <unordered_map>
namespace gl {
    namespace Texture {

	struct Texture2D {
	    Texture2D(size_t pWidth, size_t pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType);
	    size_t ID = 0;
	    size_t width = 0;
	    size_t height = 0;
	    GLenum internalFormat;
	    GLenum format;
	    GLenum type;
	};

	struct TextureBuffer {
	    TextureBuffer() :data(nullptr), width(0), height(0), channels(0) {}
	    unsigned char* data;
	    int width;
	    int height;
	    int channels;
	};

	struct TexData2D {
	    std::vector<unsigned char> data;
	    size_t width;
	    size_t height;
	};
	extern vector<Texture2D> all2DTextures;

	void loadTextureBuffer(TextureBuffer& pBuffer, std::string pFilename, int pForceChannels = 0);
	size_t createTexture2D(size_t pWidth, size_t pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType, const void* pData);
	size_t createTexture2D(std::string pFilename);
	size_t createTexture2D(TextureBuffer pBuffer);
	void setTextureWrapping(size_t pTextureIndex, size_t pWrapS, size_t pWrapT);
	void setTextureFilter(size_t pTextureIndex, size_t pMagFilter, size_t pMinFilter);
	size_t get2DTextureID(size_t pTextureIndex);
	void setTextureDirectory(std::string& pDirectory);
	void resetTextureDirectory();
	extern std::string TEXTURE_DIR;
	extern std::unordered_map<std::string, size_t> textureLookup;
    }
}