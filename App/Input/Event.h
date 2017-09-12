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
			static unsigned int create(EventType pEvent) {
				EventSlot<EventType> slt(pEvent);
				instances.push_back(slt);
				return slt.index;
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
			unsigned index;
		private:
			EventSlot(EventType pEvent) :index(TOTAL_SIGNAL_COUNT++), evnt(pEvent) {}

			static std::vector<EventSlot<EventType>> instances;
			
		};
		template<class EventType>
		std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();
			
		
	}
}