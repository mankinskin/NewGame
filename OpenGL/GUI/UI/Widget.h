#pragma once
#include "Quad.h"
#include "Element.h"
#include <tuple>
namespace gl {
	namespace GUI {


		template<size_t N, template<size_t> class F>
		struct Add_Values {
			static const size_t value = F<N - 1>::value + Add_Values<N - 1, F>::value;
		};
		template<template<size_t> class F>
		struct Add_Values<0, F> {
			static const size_t value = 0;
		};

		template<class ...Elements>
		struct WidgetPreset {
			WidgetPreset() {}
			WidgetPreset(ElementPreset<Elements>... pPresets)
				:presets(pPresets...) {}

			std::tuple<ElementPreset<Elements>...> presets;

			template<size_t Ind>
			struct Get_Quad_Count {
				enum :size_t {
					value = std::tuple_element<Ind, std::tuple<ElementPreset<Elements>...>>::type::type::QUAD_COUNT
				};
			};
			static const size_t TOTAL_QUAD_COUNT = Add_Values<sizeof...(Elements), Get_Quad_Count>::value;
		};

		template<class ...Elements>
		struct Widget {
		public:
			Widget() {}
			Widget(WidgetPreset<Elements...> pPreset)
				:elements(create_elements_from_presets<Elements...>::func(pPreset.presets))
			{	}
			Widget(ElementPreset<Elements>... pPresets)
				:elements(create_elements_from_presets<Elements...>::func(std::tuple<ElementPreset<Elements>...>(pPresets...)))
			{	}
			template<size_t N>
			auto get() {
				return std::get<N>(elements);
			}
			void move(glm::vec2 pOffset) {
				move_next<sizeof...(Elements)-1>(pOffset);
			}
			template<size_t N, class... Colors>
			void colorize(ColorPreset<Widget<Elements...>, Colors...> pColor) {
				color(std::get<N>(elements), std::get<N>(pColor.colors));
			}

			static const size_t ELEMENT_COUNT = sizeof...(Elements);
		private:
			template<size_t N>
			void move_next(glm::vec2& pOffset) {
				move_next<N - 1>(pOffset);
				move_element<N>(pOffset);
			}
			template<>
			void move_next<0>(glm::vec2& pOffset) {
				move_element<0>(pOffset);
			}
			template<size_t N>
			void move_element(glm::vec2& pOffset) {
				auto element = std::get<N>(elements);
				for (unsigned int q = 0; q < std::tuple_element<N, std::tuple<Elements...>>::type::QUAD_COUNT; ++q) {
					allQuads[element.quads[q] - 1].x += pOffset.x;
					allQuads[element.quads[q] - 1].y += pOffset.y;
				}
			}
			
			std::tuple<Elements...> elements;
		};

		template<class Widget>
		void moveWidget(Widget pWidget, glm::vec2& pOffset) {
			pWidget.move(pOffset);
		}

	}
}