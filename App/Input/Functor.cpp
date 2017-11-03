#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Functor.h"
#include "Event.h"

std::vector<void(*)(size_t)> App::Input::FunctorInternal::functorInvokers = std::vector<void(*)(size_t)>();
std::vector<void(*)()> App::Input::FunctorInternal::functorDestructors = std::vector<void(*)()>();
std::vector<std::pair<size_t, size_t>> App::Input::FunctorInternal::functorOrder = std::vector<std::pair<size_t, size_t>>();

bool App::Input::FunctorInternal::is_on(size_t pSignalIndex)
{
	return EventInternal::allSignals[pSignalIndex].signaled;
}

void App::Input::clearFunctors() {
	for (void(*&destructor)() : FunctorInternal::functorDestructors) {
		destructor();
	}
}
void App::Input::callFunctors()
{
	for (std::pair<size_t, size_t>& funi : FunctorInternal::functorOrder) {
		FunctorInternal::functorInvokers[funi.first](funi.second);
	}
}
