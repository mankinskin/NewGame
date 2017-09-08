#include "../Global/stdafx.h"
#include "Event.h"

std::unordered_map<unsigned int, std::pair<int, std::vector<unsigned int>>> App::Input::signalLockBindings;

std::vector<int> App::Input::allSignalLocks;
std::vector<unsigned int> App::Input::allSignalSlots;
std::vector<unsigned int> App::Input::allSignals;
unsigned App::Input::TOTAL_SIGNAL_COUNT;