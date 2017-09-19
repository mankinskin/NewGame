#include "../Global/stdafx.h"
#include "Event.h"

std::vector<App::Input::Signal> App::Input::allSignalSlots; // 1 = signaled
std::vector<int> App::Input::allSignalLocks; // 1 = locked 


unsigned App::Input::TOTAL_SIGNAL_COUNT;

//this might be kinda slow. it should store an array of indices for each "LockSignal" which is a signal which either lock or unlock the array of other signals
std::unordered_map<unsigned int, std::vector<unsigned int>> App::Input::signalLockBindings;
std::unordered_map<unsigned int, std::vector<unsigned int>> App::Input::signalUnlockBindings;