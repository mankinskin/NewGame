#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Event.h"

std::vector<App::Input::EventInternal::SignalSlot> App::Input::EventInternal::allSignals;
std::vector<void(*)()> App::Input::EventInternal::eventCheckers;
std::vector<void(*)()> App::Input::EventInternal::eventDestructors;

void App::Input::clearSignals() {
	EventInternal::allSignals.clear();
}
void App::Input::resetSignals()
{
	for (EventInternal::SignalSlot& signal : EventInternal::allSignals) {
		signal.signaled *= signal.remain;
	}
}
void App::Input::checkEvents() {
	for (void(*&checker)() : EventInternal::eventCheckers) {
		checker();
	}
}
void App::Input::clearEvents() {
	for (void(*&destructor)() : EventInternal::eventDestructors) {
		destructor();
	}
}