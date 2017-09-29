#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
/*
---
This is a structure which allows you to call functions of any type upon certain events. 
You define an event to wait for by creating an EventSlot<EventType>. It stores the signature of the event and receives a unique signal_index. 
Use EventSlot<EventType>::instance_count() and ::get_instance(index) to compare the stored events of each slot with your incoming events
If an Event matches an EventSlot, the Slot will set its assigned signal to 'signaled'. 
These signals can be watched by FuncSlots. FuncSlots store the function pointers and arguments of any function you want to call.
They are assigned a set of signalIDs and call their function once they find one of their signals 'signaled'.
---Usuage-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
- Optionally you can set signals to lock other signals from being signaled. This way you can synchronize many, possibly conflicting Events.
        - signal_lock(w_press_slot.signal, { s_press_slot.signal, s_release_slot.signal });
        //when 'W' is pressed, the signals of s_press and s_release are locked. they will not change anymore and any events matching these slots will be ignored.
        //You will need a signal to explicitly unlock locked signals to use them again.
        - signal_unlock(w_release_slot.signal, { s_press_slot.signal, s_release_slot.signal });
        //You have to make sure that all locks will be released at some point (unless you have specific intentions ofc), this is why you should generally use
        set_up_lock to set up both locking and unlocking signals at once.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
namespace App {
	namespace Input {

		struct Signal {
			Signal(int pOn, int pStay)
				:on(pOn), stay(pStay) {}
			Signal()
				:on(0), stay(0) {}
			
			void set(int set = 1) {
				on = set;
			}
			void set_stay(int set = 1) {
				stay = set;
			}
			void reset() {
				on = stay;
			}
			int on;
			int stay;//the value to assign to 'on' when resetting it
		};
		extern std::vector<Signal> allSignalSlots; // 1 = signaled
		extern std::vector<int> allSignalLocks; // 1 = locked 
                

		//this might be kinda slow. it should store an array of indices for each "LockSignal" which is a signal which either lock or unlock the array of other signals
		extern std::unordered_map<unsigned int, std::vector<unsigned int>> signalLockBindings;
		extern std::unordered_map<unsigned int, std::vector<unsigned int>> signalUnlockBindings;
                
                
                
                static void signal_lock(unsigned int pLockSignal, std::initializer_list<unsigned int> pTargetSignals) {
		        auto it = signalLockBindings.find(pLockSignal);
		        if (it == signalLockBindings.end()) {
		               signalLockBindings.insert(std::pair<unsigned int, std::vector<unsigned int>>(pLockSignal, std::vector<unsigned int>(pTargetSignals)));
		               return;
		        }
		        it->second.insert(it->second.end(), pTargetSignals.begin(), pTargetSignals.end());
		}
		// set pLockSignal to set the locks of pTargetSignals to pLock
		static void signal_unlock(unsigned int pUnlockSignal, std::initializer_list<unsigned int> pTargetSignals) {
			auto it = signalUnlockBindings.find(pUnlockSignal);
			if (it == signalUnlockBindings.end()) {
				signalUnlockBindings.insert(std::pair<unsigned int, std::vector<unsigned int>>(pUnlockSignal, std::vector<unsigned int>(pTargetSignals)));
				return;
			}
			it->second.insert(it->second.end(), pTargetSignals.begin(), pTargetSignals.end());
		}
                static void set_up_lock(unsigned int pLockSignal, unsigned int pUnlockSignal, std::initializer_list<unsigned int> pTargetSignals) {
                        signal_lock(pLockSignal, pTargetSignals);
                        signal_unlock(pUnlockSignal, pTargetSignals);
                }


		template<typename R, typename... Args>
		class FuncSlot {
		private:
			R(*fun)(Args...);
			std::tuple<Args...> args;
			unsigned int slot_index;
			
		public:
			std::vector<unsigned int> signal_bindings;
			static std::vector<FuncSlot<R, Args...>> instances;
			
			FuncSlot() : fun(nullptr), args(std::tuple<Args...>()), signal_bindings(std::vector<unsigned>())
			{}

			FuncSlot(R(*pF)(Args...), Args... pArgs)
				:fun(pF), args(std::forward_as_tuple(pArgs)...), signal_bindings(), slot_index(instances.size()) {

				instances.push_back(*this);
			}
			static void clear() {
				instances.clear();
			}
			static void reserve_slots(unsigned int pCount) {
				instances.reserve(pCount);
			}
			void listen(std::initializer_list<unsigned int> pSignals)
			{
				instances[slot_index].signal_bindings = std::vector<unsigned>(pSignals);
			}
			void listen(FuncSlot<R, Args...>& pSlot, std::initializer_list<unsigned int> pSignals)
			{
				instances[pSlot.slot_index].signal_bindings = std::vector<unsigned>(pSignals);
			}
			R invoke() const {
				return std::apply(fun, args);
			}
			R callFunc(Args... pArgs) const {
				return fun(pArgs...);
			}
			
		};
		template<typename R, typename... Args>
		std::vector<FuncSlot<R, Args...>> FuncSlot<R, Args...>::instances = std::vector<FuncSlot<R, Args...>>();

		template<class EventType>
		class EventSlot {
		public:
			EventSlot(EventType pEvent) :signal_index(allSignalSlots.size()), evnt(pEvent) {
                                allSignalSlots.push_back(Signal());
                                allSignalLocks.push_back(0);
				instances.push_back(*this);
			}
                        static void reserve_slots(unsigned int pCount) {
                                allSignalSlots.reserve(allSignalSlots.size() + pCount);
                                allSignalLocks.reserve(allSignalLocks.size() + pCount);
				instances.reserve(pCount);
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
			unsigned signal_index;//this event´s signal slot
		private:

			static std::vector<EventSlot<EventType>> instances;
			
		};
		template<class EventType>
		std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();
		
	}
}