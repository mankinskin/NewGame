#include "../Global/stdafx.h"
#include "Signal.h"
#include <algorithm>
std::vector<App::Input::SignalInternal::SignalSlot> App::Input::SignalInternal::allSignals;
std::vector<void(*)()> App::Input::SignalInternal::functorInvokers = std::vector<void(*)()>();
std::vector<void(*)()> App::Input::SignalInternal::functorDestructors = std::vector<void(*)()>();

bool App::Input::is_on(size_t pSignalIndex)
{
	return SignalInternal::allSignals[pSignalIndex].signaled;
}

void App::Input::clearSignals() {
	SignalInternal::allSignals.clear();
}

void App::Input::clearFunctors() {
	using namespace SignalInternal;
	for (void(*&destructor)() : functorDestructors) {
		destructor();
	}
}


void App::Input::callFunctors()
{
	using namespace SignalInternal;
	for (void(*&invoker)() : functorInvokers) {
		invoker();
	}

	//any other FuncSlot templates here

	for (SignalSlot& signal : allSignals) {
		signal.signaled *= signal.remain;
	}
}
