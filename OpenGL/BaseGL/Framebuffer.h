#pragma once

namespace gl {
	namespace Texture {
		void initGBuffer();
		extern unsigned int gBuffer;
		extern unsigned int gPosTexture;
		extern unsigned int gDepthTexture;
		extern unsigned int gNormalTexture;
		extern unsigned int gAmbientTexture;
		extern unsigned int gDiffuseTexture;
		extern unsigned int gSpecularTexture;

		void initLightFBO();
		extern unsigned int lightFBO;
		extern unsigned int lightColorTexture;
		extern unsigned int lightDepthTexture;

                void initButtonFBO();
                extern unsigned int buttonFBO;
                extern unsigned int buttonIndexTexture;
                extern unsigned int buttonDepthTexture;
	}
}
