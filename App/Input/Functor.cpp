#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Functor.h"
#include "Event.h"

std::vector<void(*)()> App::Input::FunctorInternal::functorInvokers = std::vector<void(*)()>();
std::vector<void(*)()> App::Input::FunctorInternal::functorDestructors = std::vector<void(*)()>();

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
	for (void(*&invoker)() : FunctorInternal::functorInvokers) {
		invoker();
	}
}
