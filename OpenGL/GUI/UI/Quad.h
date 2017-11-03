#pragma once
#include <glm/glm.hpp>
namespace gl {
	namespace GUI {
		const float aspect_ratio = 1600.0f / 850.0f;
		template<class Compound>
		size_t createCompound(float pPosX, float pPosY, float pWidth, float pHeight, float pMarging = 0.05f);
		size_t createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
		void reserveQuads(size_t pCount);
		void clearBuffers();
		void initQuadBuffer();
		void updateQuadBuffer();
		template<class Compound>
		size_t createCompound(float pPosX, float pPosY, float pWidth, float pHeight, float pMarging)
		{
			compounds<Compound>.emplace_back(pPosX, pPosY, pWidth, pHeight, pMarging);
		}
		struct BorderCornerQuads {
			BorderCornerQuads(float pPosX, float pPosY, float pWidth, float pHeight, float pMargin)
				:posx(pPosX), posy(pPosY), width(pWidth), height(pHeight), margin(pMargin) {}
			static const unsigned QUAD_COUNT = 9;
			void create(size_t*& pIndexBegin) {
				*(pIndexBegin++) = createQuad(posx, posy, margin, margin*aspect_ratio);
				*(pIndexBegin++) = createQuad(posx + margin, posy, width - margin *2.0f, margin*aspect_ratio);
				*(pIndexBegin++) = createQuad(posx + width - margin, posy, margin, margin*aspect_ratio);

				*(pIndexBegin++) = createQuad(posx, posy - margin*aspect_ratio, margin, height - margin *2.0f*aspect_ratio);
				*(pIndexBegin++) = createQuad(posx + margin, posy - margin*aspect_ratio, width - margin *2.0f, height - margin *2.0f*aspect_ratio);
				*(pIndexBegin++) = createQuad(posx + width - margin, posy - margin*aspect_ratio, margin, height - margin *2.0f*aspect_ratio);

				*(pIndexBegin++) = createQuad(posx, posy - (height - margin*aspect_ratio), margin, margin*aspect_ratio);
				*(pIndexBegin++) = createQuad(posx + margin, posy - (height - margin*aspect_ratio), width - margin *2.0f, margin*aspect_ratio);
				*(pIndexBegin++) = createQuad(posx + width - margin, posy - (height - margin*aspect_ratio), margin, margin*aspect_ratio);
			}
			float posx;
			float posy;
			float width;
			float height;
			float margin;
		};
		struct SimpleBorderQuads {
			SimpleBorderQuads(float pPosX, float pPosY, float pWidth, float pHeight, float pMargin)
				:posx(pPosX), posy(pPosY), width(pWidth), height(pHeight), margin(pMargin) {}
			void create(size_t*& pIndexBegin) {
				*(pIndexBegin++) = createQuad(posx, posy, width, height);
				*(pIndexBegin++) = createQuad(posx + margin, posy - margin*aspect_ratio, width - margin*2.0f, height - margin*2.0f*aspect_ratio);
			}
			static const unsigned QUAD_COUNT = 2;
			float posx;
			float posy;
			float width;
			float height;
			float margin;
		};
		struct SingleQuad {
			SingleQuad(float pPosX, float pPosY, float pWidth, float pHeight)
				:posx(pPosX), posy(pPosY), width(pWidth), height(pHeight) {}
			void create(size_t*& pIndexBegin) {
				*(pIndexBegin++) = createQuad(posx, posy, width, height);
			}
			static const unsigned QUAD_COUNT = 1;
			float posx;
			float posy;
			float width;
			float height;
		};
		template<size_t... args>
		struct arr {
			static const size_t data[sizeof...(args)];
		};

		template<size_t... args>
		const size_t arr<args...>::data[sizeof...(args)] = { args... };

		template<size_t N, template<size_t>typename F, size_t... vals>
		struct gen {
			typedef typename gen<N - 1, F, F<N - 1>::value, vals...>::result result;
		};
		template<template<size_t>typename F, size_t... vals>
		struct gen<0, F, vals...> {
			typedef arr<vals...> result;
		};

		template<size_t N, template<size_t> class F>
		struct Calc_Quad_Count {
			static const size_t value = Calc_Quad_Count<N - 1, F>::value + F<N - 1>::value;
		};
		template<template<size_t> class F>
		struct Calc_Quad_Count<0, F> {
			static const size_t value = 0;
		};

		template<class ...Elements>
		struct WidgetLayout {
			WidgetLayout(Elements... pElems)
				:elements(pElems...) {}

			void create_quads() {
				create_quads<0>(&quads[0]);//create quads and store references in quads
			}
			size_t get(size_t i) {
				return quads[i];
			}
			template<size_t Ind>
			struct Element_Quad_Count {
				enum :size_t { value = std::tuple_element<Ind, std::tuple<Elements...>>::type::QUAD_COUNT };
			};

			typedef gen<sizeof...(Elements), Element_Quad_Count> quad_counts;
			size_t get(size_t el, size_t i) {
				size_t off = 0;
				for (size_t e = 0; e < el; ++e) {
					off += quad_counts::result::data[e];
				}
				return quads[off + i];
			}

			void move(glm::vec2 pOffset) {
				for (size_t q = 0; q < QUAD_COUNT; ++q) {
					allQuads[quads[q] - 1] += glm::vec4(pOffset, 0.0f, 0.0f);
				}
			}

		private:
			template<size_t N>
			void create_quads(size_t* p) {
				std::get<N>(elements).create(p);
				create_quads<N + 1>(p);
			}
			template<>
			void create_quads<sizeof...(Elements)>(size_t* p) {	}

			static const size_t QUAD_COUNT = Calc_Quad_Count<sizeof...(Elements), Element_Quad_Count>::value;
			size_t quads[QUAD_COUNT];
			std::tuple<Elements...> elements;


		};

		template<class ...Elements>
		struct Widget : public WidgetLayout<Elements...> {
		public:
			Widget(Elements... pElems)
				:WidgetLayout<Elements...>(pElems...){}

		};

		template<class Widget>
		void moveWidget(Widget pWidget, glm::vec2& pOffset) {
			pWidget.move(pOffset);
		}

	}
}