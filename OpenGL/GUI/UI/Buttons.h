#pragma once

namespace gl {
    namespace GUI {
	void initButtonBuffer();
	void updateButtonBuffer();
	void initButtonIndexShader();
	void rasterButtons();
	void rasterButtonQuads();
	void rasterButtonLines();
	size_t addButtonQuad(size_t pQuadID);
	size_t addButtonLine(size_t pLineID);
	void hideButton(size_t pButtonID);
	void unhideButton(size_t pButtonID);
	void toggleButton(size_t pButtonID);
	void setupButtonIndexShader();
	void clearButtons();
	void fetchButtonMap();
	size_t readButtonIndexMap(size_t pPos);
	size_t readButtonIndexMap(size_t pXPos, size_t pYPos);
    }
}