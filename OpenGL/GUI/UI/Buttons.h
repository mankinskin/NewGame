#pragma once

namespace gl {
	namespace GUI {
		



		void initButtonBuffer();
		void updateButtonBuffer();
		void initButtonIndexShader();
		void rasterButtons();
		void rasterButtonQuads();
		size_t addButton(size_t pQuadID);
		size_t addButton(size_t pQuadID, size_t pButtonID);

		void setupButtonIndexShader();
		void clearButtons();
		void fetchButtonMap();
		size_t readButtonIndexMap(size_t pPos);
		size_t readButtonIndexMap(size_t pXPos, size_t pYPos);
	}
}