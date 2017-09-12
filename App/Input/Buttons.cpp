#include "../Global/stdafx.h"
#include "Buttons.h"
#include "Mouse.h"
std::vector<App::Input::ButtonCondition> App::Input::allButtonStates;
std::vector<glm::vec4> App::Input::allDetectors;

std::vector<App::Input::KeyEvent> App::Input::mouseButtonEventBuffer;
std::vector<App::Input::ButtonEvent> App::Input::buttonEventBuffer;
std::vector<int> App::Input::allButtonFlags;
std::vector<unsigned int> App::Input::allButtonQuads;

unsigned int App::Input::addButton(unsigned int pQuadIndex)
{
	allButtonQuads.push_back(pQuadIndex);
	return allButtonQuads.size() - 1;
}


void App::Input::toggleButton(unsigned int pButtonIndex) {
	allButtonFlags[pButtonIndex] = !allButtonFlags[pButtonIndex];
}

void App::Input::hideButton(unsigned int pButtonIndex) {
	allButtonFlags[pButtonIndex] = 0;
}

void App::Input::unhideButton(unsigned int pButtonIndex) {
	allButtonFlags[pButtonIndex] = 1;
}

void App::Input::fetchButtonEvents()
{
	size_t button_count = allDetectors.size();
	std::vector<ButtonEvent> evnts(button_count);
	unsigned int evnt_count = 0;
	for (size_t b = 0; b < button_count; ++b) {
		if (allButtonFlags[b]) {
			//compare all previous button states to all new ones
			ButtonCondition& prevState = allButtonStates[b];
			int inside = is_inside_quad((glm::vec2)relativeCursorPosition, allDetectors[b]);
			ButtonCondition& newState =
				ButtonCondition(mouseButtons[0].action * inside, inside);
			if (prevState != newState) {
				ButtonCondition change;
				change.action = prevState.action == newState.action ? 0 : newState.action;
				change.in = prevState.in == newState.in ? 0 : newState.in;
				evnts[evnt_count++] = ButtonEvent(b, change);
			}
			prevState = newState;
		}
	}
	if (!evnt_count)
		return;
	buttonEventBuffer.resize(evnt_count);
	std::memcpy(&buttonEventBuffer[0], &evnts[0], sizeof(ButtonEvent)*evnt_count);
}

void App::Input::checkButtonEvents()
{
	size_t buttonEventCount = buttonEventBuffer.size();
	unsigned int signalOffset = signalBuffer.size();
	signalBuffer.resize(signalOffset + buttonEventCount);
	unsigned int signalCount = 0;
	for (unsigned int e = 0; e < buttonEventCount; ++e) {
		ButtonEvent& kev = buttonEventBuffer[e];
		for (unsigned int ks = 0; ks < EventSlot<ButtonEvent>::instance_count(); ++ks) {
			EventSlot<ButtonEvent>& slot = EventSlot<ButtonEvent>::get(ks);
			if (slot.evnt == kev) {
				signalBuffer[signalOffset + signalCount++] = slot.index;
			}
		}

	}
	buttonEventBuffer.clear();
	signalBuffer.resize(signalOffset + signalCount);
}

void App::Input::loadButtons()
{
	using gl::GUI::allSizes;
	using gl::GUI::allPositions;
	using gl::GUI::allQuads;
	size_t quadCount = allButtonQuads.size();
	allDetectors.resize(quadCount);
	allButtonStates.resize(quadCount);
	allButtonFlags.resize(quadCount, 1);
	for (unsigned int q = 0; q < quadCount; ++q) {
		allDetectors[q] = glm::vec4(
			allPositions[allQuads[allButtonQuads[q]].pos].x,
			allPositions[allQuads[allButtonQuads[q]].pos].y,
			allPositions[allQuads[allButtonQuads[q]].pos].x + allSizes[allQuads[allButtonQuads[q]].size].x,
			allPositions[allQuads[allButtonQuads[q]].pos].y - allSizes[allQuads[allButtonQuads[q]].size].y);
	}
}

void App::Input::clearButtons()
{
	allButtonQuads.clear();
	allButtonStates.clear();
	allButtonFlags.clear();
	allDetectors.clear();
}