#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
/*
---
This is a structure which allows you to call functions of any type upon certain events. 
You define an event to wait for by creating an EventSlot<EventType>. It stores the signature of the event and receives a unique signal_index. 
Use EventSlot<EventType>::instance_count() and ::get_instance(index) to compare the stored events of each slot with your incoming events
If an Event matches an EventSlot, the Slot will set its assigned signal to 'signaled'. 
These signals can be watched by FuncSlots. FuncSlots store the function pointers and arguments of any function you want to call.
They are assigned a set of signalIDs and call their function once they find one of their signals 'signaled'.
---Usage-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
- create EventSlots/Signals -
        - (it is recommended to call EventSlot<EventType>::reserve_slots(slotCount); before defining slots)
        - EventSlot<KeyEvent> w_press_slot(KeyEvent(GLFW_KEY_W, 1, 0)); //creates a signal which becomes 'signaled' when KeyEvent(GLFW_KEY_W, 1, 0) appears in the eventBuffer
- create FuncSlots to call functions -
        - FuncSlot<void> moveForwardSlot(moveForward);
        - or: FuncSlot<void, vec3> moveForwardSlot(move, 0.0f, 0.0f, -1.0f);
        - or: FuncSlot<void> moveRightSlot( [](){ move(1.0f, 0.0f, 0.0f); });
- assign Signals to FuncSlots -
        - moveForwardSlot.listen({ w_press_slot });
        - moveRightSlot.listen({ d_press_slot });
        - stopXSlot.listen({ d_release_slot, a_release_slot });
---
- Optionally you can set signals to block other signals from being signaled. This way you can synchronize many, possibly conflicting Events.
        - signal_lock(w_press_slot.signal, { s_press_slot.signal, s_release_slot.signal });
        //when 'W' is pressed, the signals of s_press and s_release are blocked. they will not change anymore and any events matching these slots will be ignored.
        //You will need a signal to explicitly unlock locked signals to use them again.
        - signal_unlock(w_release_slot.signal, { s_press_slot.signal, s_release_slot.signal });
        //You have to make sure that all locks will be released at some point (unless you have specific intentions ofc), this is why you should generally use
        set_up_lock to set up both locking and unlocking signals at once.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
namespace App {
    namespace Input {
	namespace SignalInternal {

		struct SignalSlot {
			SignalSlot() :signaled(0), remain(0) {}
			int signaled = 0;
			int remain = 0;
		};
		extern std::vector<SignalSlot> allSignals;
		

		template<class EventType>
		class EventSlot {
		public:
			EventSlot(EventType pEvent, unsigned int pSignalIndex) :signalIndex(allSignals.size()), evnt(pEvent) {}

			static void reserve_slots(unsigned int pCount) {
				allSignals.reserve(allSignals.size() + pCount);
				instances.reserve(instances.size() + pCount);
			}
			static unsigned int instance_count() {
				return instances.size();
			}
			static EventSlot<EventType> get_instance(unsigned int index) {
				return instances[index];
			}
			static void clear() {
				instances.clear();
			}

			EventType evnt;
			unsigned signalIndex;//this event´s signal slot
			static std::vector<EventSlot<EventType>> instances;

		};
		template<class EventType>
		std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();


		//funcs
		
		//TO-DO: CHECK IF RETURN ARGUMENT REALLY IS NEEDED ... (why not? .. might come in use later)
		template<typename R, typename... Args>
		class FuncSlot {
		private:
			std::function<R(Args...)> fun;
			std::tuple<Args...> args;
			
		public:
			static std::vector<FuncSlot<R, Args...>> instances;
			std::vector<unsigned int> blockSignals;
			std::vector<unsigned int> unblockSignals;
			std::vector<unsigned int> signalBindings;

			bool rule = 0;//always call
			FuncSlot() : fun(), args(std::tuple<Args...>())
			{}

			FuncSlot(std::function<R(Args...)> pF, Args... pArgs)
				:fun(pF), args(std::forward_as_tuple(pArgs)...), signalBindings(){}
			static void clear() {
				instances.clear();
			}
			static void reserve_slots(unsigned int pCount) {
				instances.reserve(pCount);
			}

			static void listen(FuncSlot<R, Args...>& pSlot, std::initializer_list<unsigned int>& pSignals)
			{
				pSlot.signalBindings.insert(pSlot.signalBindings.end(), pSignals.begin(), pSignals.end());
			}
			R invoke() const {
				return std::apply(fun, args);
			}
		};
		template<typename R, typename... Args>
		std::vector<FuncSlot<R, Args...>> FuncSlot<R, Args...>::instances = std::vector<FuncSlot<R, Args...>>();
		
		
	}//end Internal

	bool is_on(unsigned int pSignalIndex);

	void clearSignals();
	//void checkSignals();
	void callFunctions();
	//FuncSlots
	template<typename R, typename... Args>
	struct Func {//interface object
		Func(unsigned int pSlotIndex) :slotIndex(pSlotIndex) {}
		unsigned int slotIndex;
		void listen(std::initializer_list<unsigned int> pSignals) {
			SignalInternal::FuncSlot<R, Args...>::listen(SignalInternal::FuncSlot<R, Args...>::instances[slotIndex], pSignals);
		}
	};
	template<typename R, typename... Args>
	void reserve_funcs(unsigned pCount) {
		SignalInternal::FuncSlot<R, Args...>::reserve_slots(pCount);
	}
	template<typename R, typename... Args>
	Func<R, Args...> create_func(R(*pF)(Args...), Args... pArgs) {
		using namespace SignalInternal;
		FuncSlot<R, Args...>::instances.push_back(FuncSlot<R, Args...>(pF, pArgs...));
		return Func<R, Args...>(FuncSlot<R, Args...>::instances.size() - 1);
	}
	
	template<typename R, typename... Args>
	void func_start_rule(Func<R, Args...> pFunc)
	{
		SignalInternal::FuncSlot<R, Args...>::instances[pFunc.slotIndex].rule = 1;
	}
	template<typename R, typename... Args>
	void func_stop_rule(Func<R, Args...> pFunc)
	{
		SignalInternal::FuncSlot<R, Args...>::instances[pFunc.slotIndex].rule = 0;
	}
	template<typename R, typename... Args>
	void func_block(Func<R, Args...> pFunc, unsigned int pBlockSignal) 
	{
		SignalInternal::FuncSlot<R, Args...>::instances[pFunc.slotIndex].blockSignals.push_back(pBlockSignal);
	}
	template<typename R, typename... Args>
	void func_unblock(Func<R, Args...> pFunc, unsigned int pUnblockSignal)
	{
		SignalInternal::FuncSlot<R, Args...>::instances[pFunc.slotIndex].unblockSignals.push_back(pUnblockSignal);
	}
	template<typename R, typename... Args>
	void set_up_func_block(Func<R, Args...> pFunc, unsigned int pBlockSignal, unsigned int pUnblockSignal)
	{
		func_block(pFunc, pBlockSignal);
		func_unblock(pFunc, pUnblockSignal);
	}
	
	//Signals
	
	template<typename EventType>
	void reserve_signals(unsigned pCount) {
		SignalInternal::EventSlot<EventType>::reserve_slots(pCount);
	}
	template<typename EventType>
	unsigned int create_signal(EventType pEvent) {
		using namespace SignalInternal;
		EventSlot<EventType>::instances.push_back(EventSlot<EventType>(pEvent, allSignals.size()));
		allSignals.push_back(SignalSlot());
		return allSignals.size() - 1;
	};
}//end Input
}//end internal