#include "Keys.h"

std::vector<App::Input::KeyEvent> App::Input::keyEventBuffer;
unsigned int App::Input::keyEventSlotOffset = 0;
unsigned int App::Input::keyEventSlotCount = 0;
void App::Input::checkKeyEvents()
{
	size_t keyEventCount = keyEventBuffer.size();
	unsigned int signalOffset = signalBuffer.size();
	signalBuffer.resize(signalOffset + keyEventCount);
	unsigned int signalCount = 0;


	for (unsigned int e = 0; e < keyEventCount; ++e) {
		KeyEvent& kev = keyEventBuffer[e];
		for (unsigned int ks = 0; ks < keyEventSlotCount; ++ks) {
			EventSlot<KeyEvent>& slot = EventSlot<KeyEvent>::get(keyEventSlotOffset + ks);//wroong
			if (slot.evnt == kev) {
				signalBuffer[signalOffset + signalCount++] = slot.signal_index;
				break;
			}
		}
	}
	keyEventBuffer.clear();
	signalBuffer.resize(signalOffset + signalCount);
}
