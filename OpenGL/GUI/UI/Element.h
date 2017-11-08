#pragma once
#include "Quad.h"
#include <tuple>
#include "Colored_Quads.h"
namespace gl {
	namespace GUI {


		const float aspect_ratio = 1600.0f / 850.0f;
		template<size_t Count>
		struct Element {
			Element() {}
			template<typename ...Quads>
			Element(Quads... pQuads)
				:quads{ pQuads... }
			{
				static_assert(sizeof...(Quads) == Count, "Element Constructor: invalid number of arguments");
			}

			template<size_t N, class... Colors>
			void colorize(ColorPreset<Element<Count>, Colors...> pColor) {
				colorQuad(quads[N], std::get<N>(pColor.colors));
			}
			static const unsigned QUAD_COUNT = Count;
			static const unsigned ELEMENT_COUNT = QUAD_COUNT;
			size_t quads[QUAD_COUNT];
		};

		template<class Element>
		struct ElementPreset {
			Element create() { return Element(); }
			using type = Element;
		};
		template<>
		struct ElementPreset<Element<9>> {
			ElementPreset() {}
			ElementPreset(float pPosX, float pPosY, float pWidth, float pHeight, float pMargin)
				:posx(pPosX), posy(pPosY), width(pWidth), height(pHeight), margin(pMargin) {}

			Element<9> create() {
				return Element<9>(
					createQuad(posx, posy, margin, margin*aspect_ratio),
					createQuad(posx + margin, posy, width - margin *2.0f, margin*aspect_ratio),
					createQuad(posx + width - margin, posy, margin, margin*aspect_ratio),
					createQuad(posx, posy - margin*aspect_ratio, margin, height - margin *2.0f*aspect_ratio),
					createQuad(posx + margin, posy - margin*aspect_ratio, width - margin *2.0f, height - margin *2.0f*aspect_ratio),
					createQuad(posx + width - margin, posy - margin*aspect_ratio, margin, height - margin *2.0f*aspect_ratio),
					createQuad(posx, posy - (height - margin*aspect_ratio), margin, margin*aspect_ratio),
					createQuad(posx + margin, posy - (height - margin*aspect_ratio), width - margin *2.0f, margin*aspect_ratio),
					createQuad(posx + width - margin, posy - (height - margin*aspect_ratio), margin, margin*aspect_ratio));
			}
			using type = Element<9>;
			float posx;
			float posy;
			float width;
			float height;
			float margin;
		};
		template<>
		struct ElementPreset<Element<2>> {
			ElementPreset() {}
			ElementPreset(float pPosX, float pPosY, float pWidth, float pHeight, float pMargin)
				:posx(pPosX), posy(pPosY), width(pWidth), height(pHeight), margin(pMargin) {}
			Element<2> create() {
				return Element<2>(
					createQuad(posx, posy, width, height),
					createQuad(posx + margin, posy - margin*aspect_ratio, width - margin*2.0f, height - margin*2.0f*aspect_ratio));
			}
			using type = Element<2>;
			float posx;
			float posy;
			float width;
			float height;
			float margin;
		};
		template<>
		struct ElementPreset<Element<1>> {
			ElementPreset() {}
			ElementPreset(float pPosX, float pPosY, float pWidth, float pHeight)
				:posx(pPosX), posy(pPosY), width(pWidth), height(pHeight) {}
			Element<1> create() {
				return Element<1>(createQuad(posx, posy, width, height));
			}
			using type = Element<1>;
			float posx;
			float posy;
			float width;
			float height;
		};


		template<size_t N, class ...Elements>
		struct extract_element_from_preset {
			static std::tuple<Elements...> func(std::tuple<ElementPreset<Elements>...> pPresets) {
				std::tuple<Elements...> out = extract_element_from_preset<N - 1, Elements...>::func(pPresets);
				std::get<N>(out) = std::get<N>(pPresets).create();
				return out;
			}
		};

		template<class ...Elements>
		struct extract_element_from_preset<0, Elements...> {
			static std::tuple<Elements...> func(std::tuple<ElementPreset<Elements>...> pPresets) {
				std::tuple<Elements...> out;
				std::get<0>(out) = std::get<0>(pPresets).create();
				return out;
			}
		};

		template<class ...Elements>
		struct create_elements_from_presets {
			static std::tuple<Elements...> func(std::tuple<ElementPreset<Elements>...> pPresets) {
				return extract_element_from_preset<sizeof...(Elements)-1, Elements...>::func(pPresets);
			}
		};

	}
}