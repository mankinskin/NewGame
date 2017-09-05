#pragma once
namespace App {
	namespace Input {
		

		extern std::vector<unsigned> allSignals;
		extern unsigned TOTAL_SIGNAL_COUNT;

		template<class EventType>
		class Signal {
		public:
			Signal(unsigned pIndex, EventType pEvent) :index(pIndex), evnt(pEvent) {}
			EventType evnt;
			unsigned index;
		};
		
		template<typename R, typename... Args>
		class FuncSlot {

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
			static void invoke_all() {
				for (FuncSlot<R, Args...>& inst : instances) {
					if (inst.should_call()) {
						inst.invoke();
					}
				}
			}
			static void bind(unsigned int pSlot, std::initializer_list<unsigned int> pSignals);
			static void bind(FuncSlot<R, Args...>& pSlot, std::initializer_list<unsigned int> pSignals);
			R invoke() const {
				return std::apply(fun, args);
			}
			R callFunc(Args... pArgs) const {
				return fun(pArgs...);
			}
			static std::vector<FuncSlot<R, Args...>> instances;
		private:
			R(*fun)(Args...);
			std::tuple<Args...> args;
			std::vector<unsigned int> signal_bindings;
			unsigned int slot_index;


			int should_call() {
				for (unsigned s : signal_bindings) {
					if (allSignals[s]) {
						return 1;
					}
				}
				return 0;
			}
		};
		template<typename R, typename... Args>
		std::vector<FuncSlot<R, Args...>> FuncSlot<R, Args...>::instances = std::vector<FuncSlot<R, Args...>>();

		template<typename R, typename ...Args>
		inline void FuncSlot<R, Args...>::bind(unsigned int pSlot, std::initializer_list<unsigned int> pSignals)
		{
			instances[pSlot].signal_bindings = std::vector<unsigned>(pSignals);
		}
		template<typename R, typename ...Args>
		inline void FuncSlot<R, Args...>::bind(FuncSlot<R, Args...>& pSlot, std::initializer_list<unsigned int> pSignals)
		{
			instances[pSlot.slot_index].signal_bindings = std::vector<unsigned>(pSignals);
		}
	}
}