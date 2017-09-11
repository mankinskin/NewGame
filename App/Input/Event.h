#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
namespace App {
	namespace Input {
		template<typename R, typename... Args>
		class FuncSlot {
		private:
			R(*fun)(Args...);
			std::tuple<Args...> args;

			unsigned int slot_index;
		public:
			FuncSlot() : fun(nullptr), args(std::tuple<Args...>()), signal_bindings(std::vector<unsigned>())
			{}

			FuncSlot(R(*pF)(Args...), Args... pArgs)
				:fun(pF), args(std::forward_as_tuple(pArgs)...), signal_bindings(), slot_index(instances.size()) {

				instances.push_back(*this);
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
			static std::vector<FuncSlot<R, Args...>> instances;
			std::vector<unsigned int> signal_bindings;
		};
		template<typename R, typename... Args>
		std::vector<FuncSlot<R, Args...>> FuncSlot<R, Args...>::instances = std::vector<FuncSlot<R, Args...>>();

		template<class EventType>
		class EventSlot {
		public:
			static unsigned int create(EventType pEvent) {
				EventSlot<EventType> slt(pEvent);
				instances.push_back(slt);
				return slt.index;
			}
			static unsigned int instance_count() {
				return instances.size();
			}

			static EventSlot<EventType>& get(unsigned int index) {
				return instances[index];
			}
			EventType evnt;
			unsigned index;
		private:
			EventSlot(EventType pEvent) :index(TOTAL_SIGNAL_COUNT++), evnt(pEvent) {}

			static std::vector<EventSlot<EventType>> instances;
		};
		template<class EventType>
		std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();


		extern std::vector<int> allSignalLocks;
		extern std::vector<unsigned> allSignalSlots;
		extern std::vector<unsigned> allSignals;//for ordered access
		extern unsigned TOTAL_SIGNAL_COUNT;
		extern std::unordered_map<unsigned int, std::pair<int, std::vector<unsigned int>>> signalLockBindings;
		
		static void signal_lock(unsigned int pLockSignal, std::initializer_list<unsigned int> pSignal, int pLock = 1) {
			auto it = signalLockBindings.find(pLockSignal);
			if (it == signalLockBindings.end()) {
				signalLockBindings.insert(std::pair<unsigned int, std::pair<int, std::vector<unsigned int>>>(pLockSignal, std::pair<int, std::vector<unsigned int>>(pLock, pSignal)));
				return;
			}
			it->second.second.insert(it->second.second.end(), pSignal.begin(), pSignal.end());
		}
		static void signal_unlock(unsigned int pLockSignal, std::initializer_list<unsigned int> pSignal) {
			signal_lock(pLockSignal, pSignal, 0);
		}
		
	}
}