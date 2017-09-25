#pragma once
namespace gl {
        namespace GUI {
                void initButtonBuffer();
                void updateButtonBuffer();
                void initButtonIndexShader();
                void rasterButtons();

                extern unsigned int buttonVAO;
                extern unsigned int buttonBuffer;
                extern unsigned int buttonIndexShader;
        }
}