#pragma once
#include <vector>
namespace gl {
        namespace GUI {
                void initButtonBuffer();
                void updateButtonBuffer();
                void initButtonIndexShader();
                void rasterButtons();
                unsigned int addButton(unsigned int pQuadIndex);
                void hideButton(unsigned int pButtonID);
                void unhideButton(unsigned int pButtonID);
                void toggleButton(unsigned int pButtonID);

                void clearButtons();
                void fetchButtonMap();

                extern std::vector<unsigned int> buttonIndexMap;
                extern std::vector<unsigned int> allButtonQuads;
                extern std::vector<int> allButtonFlags;
                extern unsigned int buttonVAO;
                extern unsigned int buttonBuffer;
                extern unsigned int buttonIndexShader;
        }
}