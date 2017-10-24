#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
namespace gl {
	namespace GUI {
		
		struct PlainColor {
			PlainColor(size_t pColorIndex)
				:colorIndex(pColorIndex) {}
			size_t colorIndex;
		};
		struct MultiColor {
			MultiColor(size_t pColorIndices[4])
				:colors{ pColorIndices[0], pColorIndices[1], pColorIndices[2], pColorIndices[3] } {}
			size_t colors[4];
		};
		struct TextureColor {
			TextureColor(size_t pUVIndex, size_t pColorIndex)
				 :uv_index(pUVIndex), color_index(pColorIndex){}
			size_t uv_index;
			size_t color_index;
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
		void uploadUVBuffer();
		size_t createAtlasUVRange(glm::vec4 pUVRange);

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
	ColorQuadTemplate<ColorPolicy>::quads.emplace_back(pQuad - 1, pColor);
}