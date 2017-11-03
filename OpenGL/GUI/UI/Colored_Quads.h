#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
namespace gl {
	namespace GUI {
		
		struct ConstColor {
			ConstColor(size_t pColorIndex)
				:colorIndex(pColorIndex) {}
			size_t colorIndex;
		};
		struct DynamicColor {
			DynamicColor(glm::vec4 pColor);
			DynamicColor(size_t pColorIndex)
				:colorIndex(pColorIndex) {}
			size_t colorIndex;
		};

		template<class ColorPolicy>
		struct ColorQuad {
			ColorQuad(size_t pQuad, ColorPolicy pColor)
				:quad(pQuad), color(pColor) {}
			size_t quad;
			ColorPolicy color;
		};

		template<class ColorPolicy>
		struct ColorQuadTemplate {
			static std::vector<gl::GUI::ColorQuad<ColorPolicy>> quads;
			static size_t elementBuffer;
			static size_t VAO;
			static size_t shader;
		};
		void initColorQuadVAOs();
		void initColorQuadShaders();
		void setupColorQuadShaders();
		void updateColorQuads();
		void renderColorQuads();
		void uploadUVRanges();
		size_t createAtlasUVRange(glm::vec4 pUVRange);
		glm::vec4& getDynamicColor(size_t pIndex);
		glm::vec4& getDynamicColor(DynamicColor pColor);

		template<class ColorPolicy>
		void colorQuad(size_t pQuad, ColorPolicy pColor);
		
		template<class ColorPolicy>
		void initColorQuadTemplateVAO();
		
		template<class ColorPolicy>
		void renderColorQuadTemplates();
		
		template<class ColorPolicy>
		void initColorQuadTemplateShader();
		
		template<class ColorPolicy>
		void updateColorQuadTemplates();
		
		template<class ColorPolicy>
		void setupColorQuadTemplateShader();
	}
}
template<class ColorPolicy>
void gl::GUI::colorQuad(size_t pQuad, ColorPolicy pColor) {

	ColorQuadTemplate<ColorPolicy>::quads.push_back(gl::GUI::ColorQuad<ColorPolicy>(pQuad, pColor));
}