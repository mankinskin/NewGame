#include "Keys.h"
#include "Signal.h"
std::vector<App::Input::KeyEvent> App::Input::keyEventBuffer;

void App::Input::checkKeyEvents()
{
	using namespace SignalInternal;
	for (KeyEvent& kev : keyEventBuffer) {
		for (unsigned int ks = 0; ks < EventSlot<KeyEvent>::instance_count(); ++ks) {
			EventSlot<KeyEvent>& slot = EventSlot<KeyEvent>::get_instance(ks);
			if (slot.evnt == kev) {
				allSignals[slot.signalIndex].signaled = true;
				break;
			}
		}
	}
	keyEventBuffer.clear();
}
