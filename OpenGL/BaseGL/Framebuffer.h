#pragma once
namespace gl {
    namespace Texture {
	void initGBuffer();
	extern size_t gBuffer;
	extern size_t gPosTexture;
	extern size_t gNormalTexture;
	extern size_t gAmbientTexture;
	extern size_t gDiffuseTexture;
	extern size_t gSpecularTexture;
	extern size_t gDepthRenderbuffer;
	void initGUIFBO();
	extern size_t guiFBO;
	extern size_t buttonIndexTexture;
	extern size_t fontColorTexture;
	extern size_t guiDepthRenderbuffer;
    }
}
