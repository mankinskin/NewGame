#pragma once
#include "Colored_Quads.h"
#include <App//Input//Event.h>
#include <App//Input//Functor.h>
#include <App//Input//Mouse.h>
#include "Quad.h"

namespace gl {
	namespace GUI {
		struct BorderCornerQuads {
			BorderCornerQuads()
			{}
			BorderCornerQuads(size_t pQuads[9])
				:quads{pQuads[0], pQuads[1], pQuads[2], pQuads[3], pQuads[4], pQuads[5], pQuads[6], pQuads[7], pQuads[8] }{}
			size_t quads[9];
		};
		struct SimpleBorderQuad {
			SimpleBorderQuad()
			{}
			SimpleBorderQuad(size_t pQuads[2])
				:quads{ pQuads[0], pQuads[1]} {}
			size_t quads[2];
		};

		template<class Construction>
		struct QuadConstruct {
			QuadConstruct(){}
			QuadConstruct(Construction pConstruction)
				:quadConstruct(pConstruction){}
			Construction quadConstruct;
		};

		struct SliderQuads {
			SliderQuads(float pPosX, float pPosY, float pWidth, float pHeight, float pSlideWidth);
			SliderQuads(size_t pBoundQuad, size_t pSlideQuad);
			size_t bound_quad;
			size_t slide_quad;
		};
		template<class ColorPolicy>//PlainColor, MultiColor, TextureColor
		struct SliderColors {
			SliderColors(size_t pBoundColor, size_t pSlideColor);
			size_t bound_color;
			size_t slide_color;
		};
		template<typename T>
		void setToDiff(T pTarg, T pOffMax, T pOffMin) {
			pTarg = pOffMax - pOffMin;
		}
		template<typename T, typename U>
		void max(T pTar, U pSrc) {
			pTar = pTar > pSrc ? pTar : pSrc;
		}
		template<typename T>
		void min(T pTar, T pSrc) {
			pTar = pTar < pSrc ? pTar : pSrc;
		}
		template<typename T>
		void setToScale(T pTar, T pPos, T pRange) {
			pTar = pPos / pRange;
		}
		template<typename T>
		void offset(T pTar, T pOff) {
			pTar += pOff;
		}
		template<typename T>
		void clampAboveZero(T pTar) {
			pTar = pTar > 0 ? pTar : 0;
		}
		struct BarSlideType {
			static float& passSlidePos(size_t pQuadIndex){
				return getQuad(pQuadIndex).size.x;
			}
			static void setupBoundFuncs(size_t pSlideQuad, size_t pBoundQuad) {
				using namespace App::Input;
				Functor<AnySignalGate, void, float&, float&> bindSlideMax(min,
					getQuad(pSlideQuad).size.x, getQuad(pBoundQuad).size.x, { Signal(1, 1).index() });
				Functor<AnySignalGate, void, float&> bindSlideMin(clampAboveZero,
					getQuad(pSlideQuad).size.x, { Signal(1, 1).index() });
			}
			static void setupSetSlidePosFunc(size_t pSlideQuad, size_t pBoundQuad, size_t pSignal) {
				using namespace App::Input;
				Functor<AnySignalGate, void, float&, float&, float&> setSlideQuadFunc(setToDiff,
					passSlidePos(pSlideQuad), relativeCursorPosition.x, getQuad(pBoundQuad).pos.x, { pSignal });
			}
			static void controlTarget(size_t pSlideQuad, size_t pBoundQuad, float& pTarget) {
				using namespace App::Input;
				Functor<AnySignalGate, void, float&, float&, float&> controlTarget(setToScale,
					pTarget, passSlidePos(pSlideQuad), getQuad(pBoundQuad).size.x, { Signal(1, 1).index() });
			}
			static float getAmt(size_t pSlideQuad, size_t pBoundQuad) {
				return getQuad(pSlideQuad).size.x / getQuad(pBoundQuad).size.x;
			}
		};

		struct PinSlideType {
			
			static float& passSlidePos(size_t pQuadIndex) {
				return getQuad(pQuadIndex).pos.x;
			}
			static void setupBoundFuncs(size_t pSlideQuad, size_t pBoundQuad) {
				using namespace App::Input;
				Functor<AnySignalGate, void, float&, float&, float&, float&> bindSlideMax([](float& pPos, float& pSize, float& pBoundPos, float& pBoundSize) { pPos = pPos < pBoundPos + pBoundSize - pSize ? pPos : pBoundPos + pBoundSize - pSize; },
					getQuad(pSlideQuad).pos.x, getQuad(pSlideQuad).size.x, getQuad(pBoundQuad).pos.x, getQuad(pBoundQuad).size.x, { Signal(1, 1).index() });
				Functor<AnySignalGate, void, float&, float&> bindSlideMin(max,
					getQuad(pSlideQuad).pos.x, getQuad(pBoundQuad).pos.x, { Signal(1, 1).index() });
			}
			static void setupSetSlidePosFunc(size_t pSlideQuad, size_t pBoundQuad, size_t pSignal) {
				using namespace App::Input;
				
				Functor<AnySignalGate, void, float&, float&> setSlideQuadFunc([](float& pPos, float& pGlobalPos) {pPos = pGlobalPos; },
					passSlidePos(pSlideQuad), relativeCursorPosition.x, { pSignal });
			}
			static float getAmt(size_t pSlideQuad, size_t pBoundQuad) {
				return std::abs((getQuad(pSlideQuad).pos.x - getQuad(pBoundQuad).pos.x)) / (getQuad(pBoundQuad).size.x - getQuad(pSlideQuad).size.x);
			}
		};
		template<class SliderType>
		void calcTarget(float& pMin, float& pMax, float& pTarget, size_t& pSlideQuad, size_t& pBoundQuad) {
				pTarget = pMin + (pMax - pMin)*SliderType::getAmt(pSlideQuad, pBoundQuad);
		}
		struct SliderControl {
			SliderControl(float pMin, float pMax, float& pTarget)
				:min(pMin), max(pMax), target(pTarget) {}
			float min;
			float max;
			float& target;
			
			template<class SliderType>
			void setup(size_t& pSlideQuad, size_t& pBoundQuad) {
				using namespace App::Input;
				Functor<AnySignalGate, void, float&, float&, float&, size_t&, size_t&> controlTarget(calcTarget<SliderType>,
					min, max, target, pSlideQuad, pBoundQuad, { Signal(1, 1).index() });
			}
		};
		template<typename Target, class ColorPolicy, class SliderType>
		struct Slider {
			Slider(SliderQuads pQuads, SliderColors<ColorPolicy> pColors, SliderControl pControl);
			SliderQuads quads;
			SliderColors<ColorPolicy> colors;
			SliderControl control;
		};
		
		template<class ColorPolicy>
		inline SliderColors<ColorPolicy>::SliderColors(size_t pBoundColor, size_t pSlideColor)
			:bound_color(pBoundColor), slide_color(pSlideColor)	{}

		

		template<typename Target, class ColorPolicy, class SliderType>
		Slider<Target, ColorPolicy, SliderType>::Slider(SliderQuads pQuads, SliderColors<ColorPolicy> pColors, SliderControl pControl)
			: quads(pQuads), colors(pColors), control(pControl)
		{
			using namespace App::Input;
			colorQuad<ColorPolicy>(quads.bound_quad, ColorPolicy(colors.bound_color));
			colorQuad<ColorPolicy>(quads.slide_quad, ColorPolicy(colors.slide_color));
			size_t button = addButtonQuad(quads.bound_quad);
			size_t button_press = EventSignal<MouseEvent>(MouseEvent(button, MouseKeyEvent(0, KeyCondition(1, 0)))).index();
			size_t button_release = EventSignal<MouseEvent>(MouseEvent(button, MouseKeyEvent(0, KeyCondition(0, 0)))).index();

			SliderType::setupSetSlidePosFunc(quads.slide_quad, quads.bound_quad, button_press);

			size_t moveSlideSignal = EventSignal<MouseEvent>(MouseEvent(button, MouseKeyEvent(0, KeyCondition(1, 0))), Signal(1).index()).index();
			EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(0, 0)), moveSlideSignal, 0);
			Functor<AnySignalGate, void, float&, float&> moveSlideQuadFunc(offset, 
				SliderType::passSlidePos(quads.slide_quad), cursorFrameDelta.x, { moveSlideSignal });

			SliderType::setupBoundFuncs(quads.slide_quad, quads.bound_quad);

			control.setup<SliderType>(quads.slide_quad, quads.bound_quad);
			
		}
	}
}
