#pragma once

namespace gl {
	namespace Texture {
		void initGBuffer();
		extern unsigned int gBuffer;
		extern unsigned int gPosTexture;
		extern unsigned int gDepthTexture;
		extern unsigned int gNormalTexture;
		extern unsigned int gAlbedoTexture;
		extern unsigned int gAmbientTexture;
		extern unsigned int gDiffuseTexture;
		extern unsigned int gSpecularTexture;
	}
}