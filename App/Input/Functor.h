#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
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
		struct AnySignalGate {

			static bool gate(std::vector<size_t>::iterator pHeadIt, std::vector<size_t>::iterator pTailIt,  bool(*check)(size_t)) {
				return std::any_of(pHeadIt, pTailIt, check);
			}
		};
		struct AllSignalGate {
			static bool gate(std::vector<size_t>::iterator pHeadIt, std::vector<size_t>::iterator pTailIt, bool(*check)(size_t)) {
				return std::all_of(pHeadIt, pTailIt, check);
			}
		};
		namespace FunctorInternal {
			template<class SignalGate, typename R, typename... Args>
			class FunctorImpl {
			private:
				R(*func)(Args...);
				std::tuple<Args...> args;
				
			public:
				static std::vector<FunctorImpl<SignalGate, R, Args...>> instances;
				std::vector<size_t> signalBindings;

				FunctorImpl() : func(nullptr), args(std::tuple<Args...>())
				{}
				FunctorImpl(R(*pF)(Args...), Args... pArgs)
					:func(pF), args(std::forward_as_tuple(pArgs...))
				{}
				FunctorImpl(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
					:func(pF), args(std::forward_as_tuple(pArgs...)), signalBindings(pSignals)
				{}
				static void clear();
				static void reserve_slots(size_t pCount);

				static void listen(FunctorImpl<SignalGate, R, Args...>& pSlot, std::initializer_list<size_t> pSignals);
				R invoke() const;
			};
			bool is_on(size_t pSignalIndex);
			template<class SignalGate, typename R, typename... Args>
			void callFunctors()//call each functor of a template if their 
			{
				for (FunctorImpl<SignalGate, R, Args...>& inst : FunctorImpl<SignalGate, R, Args...>::instances) {
					if (SignalGate::gate(inst.signalBindings.begin(), inst.signalBindings.end(), is_on)) {
						inst.invoke();
					}
				}
			}
			
			template<class SignalGate, typename R, typename... Args>
			std::vector<FunctorImpl<SignalGate, R, Args...>> FunctorImpl<SignalGate, R, Args...>::instances = std::vector<FunctorImpl<SignalGate, R, Args...>>();

			extern std::vector<void(*)()> functorInvokers;//invokes all functor templates
			extern std::vector<void(*)()> functorDestructors;//destroys all functor templates
		}//end Internal

		//
		//
		template<class SignalGate, typename R, typename... Args>
		struct Functor {//Handle
			Functor(R(*pF)(Args...), Args... pArgs);
			Functor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals);
			void listen(std::initializer_list<size_t> pSignals);
			size_t slotIndex;
		private:
			void createFunctor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals = std::initializer_list<size_t>());
		};

		void callFunctors();
		void clearFunctors();

		template<class SignalGate, typename R, typename... Args>
		void reserve_functors(size_t pCount) {
			FunctorInternal::FunctorImpl<SignalGate, R, Args...>::reserve_slots(pCount);
		}


	}//end Input
}//end App

template<class SignalGate, typename R, typename... Args>
App::Input::Functor<SignalGate, R, Args...>::Functor(R(*pF)(Args...), Args... pArgs)
	:slotIndex(App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., std::initializer_list<size_t>());
}
template<class SignalGate, typename R, typename... Args>
App::Input::Functor<SignalGate, R, Args...>::Functor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
	:slotIndex(App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., pSignals);
}

template<class SignalGate, typename R, typename... Args>
void App::Input::Functor<SignalGate, R, Args...>::createFunctor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
{
	using namespace FunctorInternal;
	if (FunctorImpl<SignalGate, R, Args...>::instances.size() == 0) {
		functorInvokers.push_back(callFunctors<SignalGate, R, Args...>);
		functorDestructors.push_back(FunctorImpl<SignalGate, R, Args...>::clear);
	}
	FunctorImpl<SignalGate, R, Args...>::instances.emplace_back(pF, pArgs..., pSignals);
	
}
template<class SignalGate, typename R, typename... Args>
void App::Input::Functor<SignalGate, R, Args...>::listen(std::initializer_list<size_t> pSignals) {
	FunctorInternal::FunctorImpl<SignalGate, R, Args...>::listen(FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances[slotIndex], pSignals);
}

template<class SignalGate, typename R, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::clear() {
	instances.clear();
}
template<class SignalGate, typename R, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::reserve_slots(size_t pCount) {
	instances.reserve(pCount);
}
template<class SignalGate, typename R, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::listen(FunctorImpl<SignalGate, R, Args...>& pSlot, std::initializer_list<size_t> pSignals)
{
	pSlot.signalBindings.insert(pSlot.signalBindings.end(), pSignals.begin(), pSignals.end());
}
template<class SignalGate, typename R, typename... Args>
R App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::invoke() const {
	return std::apply(func, args);
}



