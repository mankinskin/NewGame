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
		template<typename Target, class ColorPolicy>
		struct Slider {
			Slider(SliderQuads pQuads, SliderColors<ColorPolicy> pColors, Target& pTargetVal);
			SliderQuads quads;
			SliderColors<ColorPolicy> colors;
			Target& target;
		};
		
		template<class ColorPolicy>
		inline SliderColors<ColorPolicy>::SliderColors(size_t pBoundColor, size_t pSlideColor)
			:bound_color(pBoundColor), slide_color(pSlideColor)	{}

		
		template<typename Target, class ColorPolicy>
		Slider<Target, ColorPolicy>::Slider(SliderQuads pQuads, SliderColors<ColorPolicy> pColors, Target & pTargetVal)
			: quads(pQuads), colors(pColors), target(pTargetVal)
		{
			//CLEAN UP YOUR SHIT
			using namespace App::Input;
			void(*setNewSlidePos)(float&, float&, float&) = [](float& pVal, float& pNew, float& pNewSub) { pVal = pNew - pNewSub; };
			void(*maxFloat_noref)(float&, float) = [](float& pTarget, float pSource) { pTarget = pTarget > pSource ? pTarget : pSource; };
			void(*maxFloat)(float&, float&) = [](float& pTarget, float& pSource) { pTarget = pTarget > pSource ? pTarget : pSource; };
			void(*minFloat)(float&, float&) = [](float& pTarget, float& pSource) { pTarget = pTarget < pSource ? pTarget : pSource; };
			void(*setFloatToScale)(float&, float&, float&) = [](float& pVal, float& pRange, float& pPos) { pVal = (pPos) / pRange; };
			void(*offsetFloat)(float&, float&) = [](float& pVal, float& pDelta) { pVal += pDelta; };
	
	
			colorQuad<ColorPolicy>(quads.bound_quad, ColorPolicy(colors.bound_color));
			colorQuad<ColorPolicy>(quads.slide_quad, ColorPolicy(colors.slide_color));
			size_t button = addButtonQuad(quads.bound_quad);
			size_t button_press = EventSignal<MouseEvent>(MouseEvent(button, MouseKeyEvent(0, KeyCondition(1, 0)))).index();
			size_t button_release = EventSignal<MouseEvent>(MouseEvent(button, MouseKeyEvent(0, KeyCondition(0, 0)))).index();

			Functor<AnySignalGate, void, float&, float&, float&> setSlideQuadFunc
			(*setNewSlidePos, getQuad(quads.slide_quad).size.x, relativeCursorPosition.x, getQuad(quads.bound_quad).pos.x, { button_press });

			size_t moveSlideSignal = EventSignal<MouseEvent>(MouseEvent(button, MouseKeyEvent(0, KeyCondition(1, 0))), Signal(1).index()).index();
			EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(0, 0)), moveSlideSignal, 0);
			Functor<AnySignalGate, void, float&, float&> moveSlideQuadFunc(*offsetFloat, getQuad(quads.slide_quad).size.x, cursorFrameDelta.x, { moveSlideSignal });


			Functor<AnySignalGate, void, float&, float&> bindSlideMax(*minFloat, getQuad(quads.slide_quad).size.x, getQuad(quads.bound_quad).size.x, { Signal(1, 1).index() });
			Functor<AnySignalGate, void, float&, float> bindSlideMin(*maxFloat_noref, getQuad(quads.slide_quad).size.x, 0.0f, { Signal(1, 1).index() });
			Functor<AnySignalGate, void, float&, float&, float&> controlTarget(*setFloatToScale, target, getQuad(quads.bound_quad).size.x, getQuad(quads.slide_quad).size.x, { Signal(1, 1).index() });
		}
	}
}
