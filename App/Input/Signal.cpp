#include "../Global/stdafx.h"
#include "Signal.h"
#include <algorithm>
std::vector<App::Input::SignalInternal::SignalSlot> App::Input::SignalInternal::allSignals;



bool App::Input::is_on(size_t pSignalIndex)
{
    return SignalInternal::allSignals[pSignalIndex].signaled;
}

void App::Input::clearSignals() {
    SignalInternal::allSignals.clear();
}

//void App::Input::checkSignals() {
//	using namespace intrnl;
//
//	//at this point all signals occurred this frame are buffered in signalBuffer
//	for (size_t& sig : signalBuffer) {
//		if (!allSignalSlots[sig].blocked) {
//			allSignalSlots[sig].on = 1;//set signal if not blocked
//		}
//		else {//if blocked, check next frame again
//			rejected.push_back(sig);
//		}
//	}
//	signalBuffer.clear();
//}

void App::Input::callFunctions()
{
    using namespace SignalInternal;
    for (FuncSlot<void>& inst : FuncSlot<void>::instances) {

	if (std::any_of(inst.signalBindings.begin(), inst.signalBindings.end(), is_on) || inst.rule) {
	    inst.invoke();
	}
    }
    for (FuncSlot<void, size_t>& inst : FuncSlot<void, size_t>::instances) {

	if (std::any_of(inst.signalBindings.begin(), inst.signalBindings.end(), is_on) || inst.rule) {
	    inst.invoke();
	}
    }

    for (FuncSlot<void, Func<void>>& inst : FuncSlot<void, Func<void>>::instances) {

	if (std::any_of(inst.signalBindings.begin(), inst.signalBindings.end(), is_on) || inst.rule) {
	    inst.invoke();
	}
    }
    for (FuncSlot<void, Func<void, size_t>>& inst : FuncSlot<void, Func<void, size_t>>::instances) {

	if (std::any_of(inst.signalBindings.begin(), inst.signalBindings.end(), is_on) || inst.rule) {
	    inst.invoke();
	}
    }

    //any other FuncSlot templates here

    for (SignalSlot& signal : allSignals) {
	signal.signaled *= signal.remain;
    }
}



