#pragma once
namespace App {
	namespace Input {
		namespace EventInternal {

			struct SignalSlot {
				SignalSlot(int pRemain = 0, int pStartWith = 0)
					:signaled(pStartWith), remain(pRemain)
				{}
				int signaled;
				int remain;
			};

			template<class EventType>
			class EventSlot {
			public:
				EventSlot(EventType pEvent, size_t pSignalIndex, int pSetTo = 1)
					:event_signature(pEvent), signalIndex(pSignalIndex), set_to(pSetTo) {}

				static void reserve_slots(size_t pCount);
				static size_t instance_count();
				static EventSlot<EventType>& get_slot(size_t index);
				static void clear();
				static void setSignals();
				static std::vector<EventSlot<EventType>> instances;
				static std::vector<EventType> eventBuffer;
			private:
				int set_to = 1;//set signal to this
				EventType event_signature;
				size_t signalIndex;
			};
			template<class EventType>
			std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();

			template<class EventType>
			std::vector<EventType> EventSlot<EventType>::eventBuffer = std::vector<EventType>();

			extern std::vector<SignalSlot> allSignals;

			extern std::vector<void(*)()> eventCheckers;//checks all EventSlot templates
			extern std::vector<void(*)()> eventDestructors;//destroys all EventSlot templates

		}
		void checkEvents();
		void clearEvents();
		void clearSignals();
		void resetSignals();

		struct Signal {
			Signal(int pRemain = 0, int pStartWith = 0)
				:slot_index(EventInternal::allSignals.size())
			{
				EventInternal::allSignals.emplace_back(pRemain, pStartWith);
			}
			size_t index() {
				return slot_index;
			}
		private:
			size_t slot_index;
		};

		template<class EventType>
		struct EventSignal {
			EventSignal(EventType pEvent, size_t pSignalIndex = Signal().index(), int pSetTo = 1);
			static void reserve(size_t pCount);
			size_t index();
		private:
			size_t signal_index;
		};
		template<class EventType>
		void pushEvent(EventType pEvent) {
			EventInternal::EventSlot<EventType>::eventBuffer.push_back(pEvent);
		}
		template<class EventType>
		size_t EventSignal<EventType>::index() {
			return signal_index;
		}
		template<class EventType>
		void EventSignal<EventType>::reserve(size_t pCount) {
			EventInternal::EventSlot<EventType>::reserve_slots(pCount);
		}
		template<class EventType>
		EventSignal<EventType>::EventSignal(EventType pEvent, size_t pSignalIndex, int pSetTo)
			:signal_index(pSignalIndex)
		{
			using namespace EventInternal;
			if (EventSlot<EventType>::instances.size() == 0) {
				eventCheckers.push_back(EventSlot<EventType>::setSignals);
				eventDestructors.push_back(EventSlot<EventType>::clear);
			}
			EventInternal::EventSlot<EventType>::instances.emplace_back(pEvent, signal_index, pSetTo);
		}
		template<class PressEventType, class ReleaseEventType>
		size_t create_button_signal(PressEventType&& pPressEvent, ReleaseEventType&& pReleaseEvent);
	}
}
template<typename EventType>
size_t App::Input::EventInternal::EventSlot<EventType>::instance_count() {
	return instances.size();
}
template<typename EventType>
App::Input::EventInternal::EventSlot<EventType>& App::Input::EventInternal::EventSlot<EventType>::get_slot(size_t index) {
	return instances[index];
}
template<typename EventType>
void App::Input::EventInternal::EventSlot<EventType>::clear() {
	instances.clear();
}
template<typename EventType>
void App::Input::EventInternal::EventSlot<EventType>::reserve_slots(size_t pCount) {
	allSignals.reserve(allSignals.size() + pCount);
	instances.reserve(instances.size() + pCount);
}
template<class EventType>
void App::Input::EventInternal::EventSlot<EventType>::setSignals() {
	for (EventType& ev : EventSlot<EventType>::eventBuffer) {
		for (size_t sl = 0; sl < EventSlot<EventType>::instance_count(); ++sl) {
			EventSlot<EventType>& slot = EventSlot<EventType>::get_slot(sl);
			if (slot.event_signature == ev) {
				allSignals[slot.signalIndex].signaled = slot.set_to;
			}
		}
	}
	EventSlot<EventType>::eventBuffer.clear();
}

template<class PressEventType, class ReleaseEventType>
size_t App::Input::create_button_signal(PressEventType&& pPressEvent, ReleaseEventType&& pReleaseEvent) {
	size_t signal_id = EventSignal<PressEventType>(pPressEvent, Signal(1).index()).index();
	EventSignal<ReleaseEventType>(pReleaseEvent, signal_id, 0).index();
	return signal_id;
}