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

//Function policies




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
				EventSlot(EventType pEvent, size_t pSignalIndex) :signalIndex(allSignals.size()), evnt(pEvent) {}

				static void reserve_slots(size_t pCount) {
					allSignals.reserve(allSignals.size() + pCount);
					instances.reserve(instances.size() + pCount);
				}
				static size_t instance_count() {
					return instances.size();
				}
				static EventSlot<EventType> get_instance(size_t index) {
					return instances[index];
				}
				static void clear() {
					instances.clear();
				}

				EventType evnt;
				size_t signalIndex;//this event´s signal slot
				static std::vector<EventSlot<EventType>> instances;

			};


			template<class EventType>
			std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();


			template<typename R, typename... Args>
			class Functor {
			private:
				std::function<R(Args...)> func;
				std::tuple<Args...> args;

			public:
				static std::vector<Functor<R, Args...>> instances;
				std::vector<size_t> signalBindings;

				bool rule;//always call
				Functor() : func(), args(std::tuple<Args...>()), rule(false)
				{}
				Functor(std::function<R(Args...)> pF, Args... pArgs)
					:func(pF), args(std::forward_as_tuple(pArgs...)), signalBindings(), rule(false)
				{}


				static void clear() {
					instances.clear();
				}
				static void reserve_slots(size_t pCount) {
					instances.reserve(pCount);
				}

				static void listen(Functor<R, Args...>& pSlot, std::initializer_list<size_t>& pSignals)
				{
					pSlot.signalBindings.insert(pSlot.signalBindings.end(), pSignals.begin(), pSignals.end());
				}
				R invoke() const {
					return std::apply(func, args);
				}
			};

			template<typename R, typename... Args>
			std::vector<Functor<R, Args...>> Functor<R, Args...>::instances = std::vector<Functor<R, Args...>>();

			template<typename R, typename... Args>
			void callFunctors()//calls all the functors of a template(if any of their signals are on)
			{
				for (SignalInternal::Functor<R, Args...>& inst : SignalInternal::Functor<R, Args...>::instances) {
					if (std::any_of(inst.signalBindings.begin(), inst.signalBindings.end(), is_on) || inst.rule) {
						inst.invoke();
					}
				}
			}

			extern std::vector<void(*)()> functorInvokers;//invokes all functor templates
		}//end Internal
		template<typename T>
		struct Min {
			Min(T& LHS, T& RHS)
				:lhs(LHS), rhs(RHS) {}

			T& lhs;
			T& rhs;
			inline T& operator()() {
				return lhs < rhs ? lhs : rhs;
			}
			template<typename L, typename R>
			static L func(L&& pLhs, R&& pRhs) {
				return pLhs < pRhs ? pLhs : pRhs;
			}

		};
		template<typename T>
		struct Max {
			Max(T& LHS, T& RHS)
				:lhs(LHS), rhs(RHS) {}


			T& lhs;
			T& rhs;
			inline T& operator()() {
				return lhs > rhs ? lhs : rhs;
			}
			template<typename L, typename R>
			static L func(L&& pLhs, R&& pRhs) {
				return pLhs > pRhs ? pLhs : pRhs;
			}
		};
		template<typename T>
		struct Add {
			Add(T& LHS, T& RHS)
				:lhs(LHS), rhs(RHS) {}

			T& lhs;
			T& rhs;
			inline T operator()() {
				return lhs + rhs;
			}
			template<typename L, typename R>
			static L func(L&& pLhs, R&& pRhs) {
				return pLhs + pRhs;
			}
		};
		template<typename T>
		struct Substract {
			Substract(T& LHS, T& RHS)
				:lhs(LHS), rhs(RHS) {}

			T& lhs;
			T& rhs;
			inline T operator()() {
				return lhs - rhs;
			}
			template<typename L, typename R>
			static L func(L&& pLhs, R&& pRhs) {
				return pLhs - pRhs;
			}
		};
		template<typename T>
		struct Pass {

			Pass(T& VAL)
				:lhs(VAL) {}

			T& lhs;
			inline T& operator()() {
				return lhs;
			}
			template<typename L, typename R>
			static L func(L&& pLhs, R&& pDummy) {
				return pLhs;
			}
		};
		template<typename T>
		struct Set {
			Set(T& LHS, T& RHS)
				:lhs(LHS), rhs(RHS) {}

			T& lhs;
			T& rhs;
			inline T& operator()() {
				lhs = rhs;
				return lhs;
			}
			template<typename L, typename R>
			static L func(L&& pTarget, R&& pSource) {
				pTarget = pSource;
				return pTarget;
			}
		};
		template<typename T, template<typename> class FuncPolicy, class LhsSource, class RhsSource>
		struct Operation {
			Operation(LhsSource pLhsSource, RhsSource pRhsSource)
				:lhs(pLhsSource), rhs(pRhsSource) {
			}
			T operator()() {
				return FuncPolicy<T>::func(lhs(), rhs());
			}
			LhsSource lhs;
			RhsSource rhs;
		};


		template<typename R, typename... Args>
		struct Func {
			
			Func(std::function<R(Args...)> pF, Args... pArgs)
				:slotIndex(SignalInternal::Functor<R, Args...>::instances.size())
			{
				if (SignalInternal::Functor<R, Args...>::instances.size() == 0) {
					SignalInternal::functorInvokers.push_back(SignalInternal::callFunctors<R, Args...>);
				}
				SignalInternal::Functor<R, Args...>::instances.emplace_back(pF, pArgs...);
			}
			Func(R(*pF)(Args...), Args... pArgs)
				:slotIndex(SignalInternal::Functor<R, Args...>::instances.size())
			{
				if (!slotIndex) {//if this is the first slot of this template
					SignalInternal::functorInvokers.push_back(SignalInternal::callFunctors<R, Args...>);
				}
				SignalInternal::Functor<R, Args...>::instances.emplace_back(pF, pArgs...);
			}

			size_t slotIndex;
			void listen(std::initializer_list<size_t> pSignals) {
				SignalInternal::Functor<R, Args...>::listen(SignalInternal::Functor<R, Args...>::instances[slotIndex], pSignals);
			}
		};

		bool is_on(size_t pSignalIndex);
		void clearSignals();
		void callFunctions();

		template<typename R, typename... Args>
		void reserve_funcs(size_t pCount) {
			SignalInternal::Functor<R, Args...>::reserve_slots(pCount);
		}

		template<typename R, typename... Args>
		void func_start_rule(Func<R, Args...> pFunc)
		{
			SignalInternal::Functor<R, Args...>::instances[pFunc.slotIndex].rule = 1;
		}
		template<typename R, typename... Args>
		void func_stop_rule(Func<R, Args...> pFunc)
		{
			SignalInternal::Functor<R, Args...>::instances[pFunc.slotIndex].rule = 0;
		}
		template<typename R, typename... Args>
		void func_block(Func<R, Args...> pFunc, size_t pBlockSignal)
		{
			SignalInternal::Functor<R, Args...>::instances[pFunc.slotIndex].blockSignals.push_back(pBlockSignal);
		}
		template<typename R, typename... Args>
		void func_unblock(Func<R, Args...> pFunc, size_t pUnblockSignal)
		{
			SignalInternal::Functor<R, Args...>::instances[pFunc.slotIndex].unblockSignals.push_back(pUnblockSignal);
		}
		template<typename R, typename... Args>
		void set_up_func_block(Func<R, Args...> pFunc, size_t pBlockSignal, size_t pUnblockSignal)
		{
			func_block(pFunc, pBlockSignal);
			func_unblock(pFunc, pUnblockSignal);
		}
		//Signals

		template<typename EventType>
		void reserve_signals(size_t pCount) {
			SignalInternal::EventSlot<EventType>::reserve_slots(pCount);
		}
		template<typename EventType>
		size_t create_signal(EventType pEvent) {
			using namespace SignalInternal;
			EventSlot<EventType>::instances.emplace_back(pEvent, allSignals.size());
			allSignals.emplace_back();
			return allSignals.size() - 1;
		};
	}//end Input
}//end internal