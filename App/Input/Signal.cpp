#include "../Global/stdafx.h"
#include "Signal.h"

std::vector<App::Input::Signal> App::Input::allSignalSlots;
std::vector<int> App::Input::allSignalLocks;

std::unordered_map<unsigned int, std::vector<unsigned int>> App::Input::signalLockBindings;
std::unordered_map<unsigned int, std::vector<unsigned int>> App::Input::signalUnlockBindings;