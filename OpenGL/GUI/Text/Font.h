#pragma once
#include <glm\glm.hpp>

namespace gl {
	namespace GUI {
		namespace Text {
			
			void initFontShader();
			void initFontVAO();
			void updateCharStorage();
			void clearCharStorage();
			void revalidateFontStringIndices();
		}
	}
}