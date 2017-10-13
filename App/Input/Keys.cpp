#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Keys.h"
#include "Signal.h"
#include "Input.h"
std::vector<App::Input::KeyEvent> App::Input::keyEventBuffer;

void App::Input::checkKeyEvents()
{
    using namespace SignalInternal;
    for (KeyEvent& kev : keyEventBuffer) {
	for (size_t ks = 0; ks < EventSlot<KeyEvent>::instance_count(); ++ks) {
	    EventSlot<KeyEvent>& slot = EventSlot<KeyEvent>::get_instance(ks);
	    if (slot.evnt == kev) {
		allSignals[slot.signalIndex].signaled = true;
		break;
	    }
	}
    }
    keyEventBuffer.clear();
}
