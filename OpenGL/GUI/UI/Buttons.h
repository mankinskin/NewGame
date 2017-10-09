#pragma once

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
		void setupButtonIndexShader();
                void clearButtons();
                void fetchButtonMap();
		unsigned int readButtonIndexMap(unsigned int pPos);
		unsigned int readButtonIndexMap(unsigned int pXPos, unsigned int pYPos);
        }
}