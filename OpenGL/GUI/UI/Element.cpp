#include "stdafx.h"
#include "..//..//Global/stdafx.h"
#include "Element.h"

gl::GUI::SliderQuads::SliderQuads(float pPosX, float pPosY, float pWidth, float pHeight, float pSlideWidth)
	:bound_quad(createQuad(pPosX, pPosY, pWidth, pHeight)), slide_quad(createQuad(pPosX, pPosY, pSlideWidth, pHeight))
{}

gl::GUI::SliderQuads::SliderQuads(size_t pBoundQuad, size_t pSlideQuad)
	: bound_quad(pBoundQuad), slide_quad(pSlideQuad){}

