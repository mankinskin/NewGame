#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
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

		
		extern unsigned TOTAL_SIGNAL_COUNT;

		//this might be kinda slow. it should store an array of indices for each "LockSignal" which is a signal which either lock or unlock the array of other signals
		extern std::unordered_map<unsigned int, std::vector<unsigned int>> signalLockBindings;
		extern std::unordered_map<unsigned int, std::vector<unsigned int>> signalUnlockBindings;

		// set pLockSignal to set the locks of pTargetSignals to pLock
		static void signal_unlock(unsigned int pLockSignal, std::initializer_list<unsigned int> pTargetSignals) {
			auto it = signalUnlockBindings.find(pLockSignal);
			if (it == signalUnlockBindings.end()) {
				signalUnlockBindings.insert(std::pair<unsigned int, std::vector<unsigned int>>(pLockSignal, std::vector<unsigned int>(pTargetSignals)));
				return;
			}
			it->second.insert(it->second.end(), pTargetSignals.begin(), pTargetSignals.end());
		}
		static void signal_lock(unsigned int pLockSignal, std::initializer_list<unsigned int> pTargetSignals) {
			auto it = signalLockBindings.find(pLockSignal);
			if (it == signalLockBindings.end()) {
				signalLockBindings.insert(std::pair<unsigned int, std::vector<unsigned int>>(pLockSignal, std::vector<unsigned int>(pTargetSignals)));
				return;
			}
			it->second.insert(it->second.end(), pTargetSignals.begin(), pTargetSignals.end());
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
			EventSlot(unsigned int& pIndexRef, EventType pEvent) :signal_index(TOTAL_SIGNAL_COUNT++), evnt(pEvent) {
				pIndexRef = signal_index;
				instances.push_back(*this);
			}
			static void reserve_slots(unsigned int pCount) {
				instances.reserve(pCount);
			}
			static unsigned int instance_count() {
				return instances.size();
			}
			static void clear() {
				instances.clear();
			}
			static EventSlot<EventType>& get(unsigned int index) {
				return instances[index];
			}
			EventType evnt;
			unsigned signal_index;
		private:

			static std::vector<EventSlot<EventType>> instances;
			
		};
		template<class EventType>
		std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();
			
		
	}
}