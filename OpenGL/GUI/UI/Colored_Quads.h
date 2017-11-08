#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <tuple>
namespace gl {
	namespace GUI {

		template<class Element, class... Colors>
		struct ColorPreset {
			static_assert(Element::ELEMENT_COUNT == sizeof...(Colors), "ColorPreset: Color count does not match the Element Count of the target");
			ColorPreset(Colors... pColors)
				:colors(pColors...) {}
			std::tuple<Colors...> colors;
		};
		template<size_t N, class T, class... Colors>
		struct color_next {
			static void func(T& pE, ColorPreset<T, Colors...> pColors) {
				color_next<N - 1, T, Colors...>::func(pE, pColors);
				pE.colorize<N - 1, Colors...>(pColors);
			}
		};
		template<class T, class... Colors>
		struct color_next<0, T, Colors...> {
			static void func(T& pE, ColorPreset<T, Colors...> pColors) {
			}
		};
		template<class T, class... Colors>
		void color(T pElement, ColorPreset<T, Colors...> pColor) {
			static_assert(T::ELEMENT_COUNT == sizeof...(Colors), "colorElement: Color count does not match the Quad Count of the Element");
			
			color_next<T::ELEMENT_COUNT, T, Colors...>::func(pElement, pColor);
		}

		
		struct ConstColor {
			ConstColor(size_t pColorIndex)
				:colorIndex(pColorIndex) {}
			size_t colorIndex;
		};

		struct Color {
			Color(glm::vec4 pColor);
			Color(size_t pColorIndex);
			glm::vec4 color;
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