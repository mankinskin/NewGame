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
			


			template<class EventType>
			class EventSlot {
			public:
				EventSlot(EventType pEvent, size_t pSignalIndex) :signalIndex(allSignals.size()), evnt(pEvent) {}

				static void reserve_slots(size_t pCount);
				static size_t instance_count();
				static EventSlot<EventType> get_instance(size_t index);
				static void clear();

				EventType evnt;
				size_t signalIndex;//this event´s signal slot
				static std::vector<EventSlot<EventType>> instances;

			};
			

			template<typename R, typename... Args>
			class FunctorImpl {
			private:
				R(*func)(Args...);
				std::tuple<Args...> args;

			public:
				static std::vector<FunctorImpl<R, Args...>> instances;
				std::vector<size_t> signalBindings;

				bool rule;//always call
				FunctorImpl() : func(nullptr), args(std::tuple<Args...>()), rule(false)
				{}
				FunctorImpl(R(*pF)(Args...), Args... pArgs)
					:func(pF), args(std::forward_as_tuple(pArgs...)), rule(false)
				{}
				FunctorImpl(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
					:func(pF), args(std::forward_as_tuple(pArgs...)), signalBindings(pSignals), rule(false)
				{}
				static void clear();
				static void reserve_slots(size_t pCount);

				static void listen(FunctorImpl<R, Args...>& pSlot, std::initializer_list<size_t> pSignals);
				R invoke() const;
			};
			template<typename R, typename... Args>
			void callFunctors()//call each functor of a template if any of its signals is set
			{
				for (SignalInternal::FunctorImpl<R, Args...>& inst : SignalInternal::FunctorImpl<R, Args...>::instances) {
					if (std::any_of(inst.signalBindings.begin(), inst.signalBindings.end(), is_on) || inst.rule) {
						inst.invoke();
					}
				}
			}

			template<class EventType>
			std::vector<EventSlot<EventType>> EventSlot<EventType>::instances = std::vector<EventSlot<EventType>>();
			
			template<typename R, typename... Args>
			std::vector<FunctorImpl<R, Args...>> FunctorImpl<R, Args...>::instances = std::vector<FunctorImpl<R, Args...>>();

			extern std::vector<SignalSlot> allSignals;
			extern std::vector<void(*)()> functorInvokers;//invokes all functor templates
			extern std::vector<void(*)()> functorDestructors;//destroys all functor templates
		}//end Internal
		template<typename R, typename... Args>
		struct Functor {//Handle
			Functor(R(*pF)(Args...), Args... pArgs);
			Functor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals);
			void listen(std::initializer_list<size_t> pSignals);
			size_t slotIndex;
		private:
			void createFunctor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals = std::initializer_list<size_t>());
		};

		bool is_on(size_t pSignalIndex);
		void clearSignals();
		void callFunctors();
		void clearFunctors();
		template<typename R, typename... Args>
		void reserve_functors(size_t pCount) {
			SignalInternal::FunctorImpl<R, Args...>::reserve_slots(pCount);
		}

		template<typename R, typename... Args>
		void functor_start_rule(Functor<R, Args...> pFunc)
		{
			SignalInternal::FunctorImpl<R, Args...>::instances[pFunc.slotIndex].rule = 1;
		}
		template<typename R, typename... Args>
		void functor_stop_rule(Functor<R, Args...> pFunc)
		{
			SignalInternal::FunctorImpl<R, Args...>::instances[pFunc.slotIndex].rule = 0;
		}
		template<typename R, typename... Args>
		void make_default_rule_functor(Functor<R, Args...>&& pFunctor)
		{
			functor_start_rule(pFunctor);
		}
		template<typename R, typename... Args>
		void make_default_rule_functor(R(*pF)(Args...), Args... pArgs)
		{
			make_default_rule_functor<R, Args...>(Functor<R, Args...>(pF, pArgs...));
		}
		template<typename R, typename... Args>
		void setup_functor_rule(Functor<R, Args...> pFunctor, std::initializer_list<size_t> pStartSignals, std::initializer_list<size_t> pStopSignals)
		{
			Functor<void, Functor<R, Args...>> start(functor_start_rule<R, Args...>, pFunctor, pStartSignals);
			Functor<void, Functor<R, Args...>> stop(functor_stop_rule<R, Args...>, pFunctor, pStopSignals);
		}

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
}//end App

template<typename R, typename... Args>
App::Input::Functor<R, Args...>::Functor(R(*pF)(Args...), Args... pArgs)
	:slotIndex(App::Input::SignalInternal::FunctorImpl<R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., std::initializer_list<size_t>());
}
template<typename R, typename... Args>
App::Input::Functor<R, Args...>::Functor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
	:slotIndex(App::Input::SignalInternal::FunctorImpl<R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., pSignals);
}

template<typename R, typename... Args>
void App::Input::Functor<R, Args...>::createFunctor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
{
	if (SignalInternal::FunctorImpl<R, Args...>::instances.size() == 0) {
		SignalInternal::functorInvokers.push_back(SignalInternal::callFunctors<R, Args...>);
		SignalInternal::functorDestructors.push_back(SignalInternal::FunctorImpl<R, Args...>::clear);
	}
	SignalInternal::FunctorImpl<R, Args...>::instances.emplace_back(pF, pArgs..., pSignals);
}
template<typename EventType>
size_t App::Input::SignalInternal::EventSlot<EventType>::instance_count() {
	return instances.size();
}
template<typename EventType>
App::Input::SignalInternal::EventSlot<EventType> App::Input::SignalInternal::EventSlot<EventType>::get_instance(size_t index) {
	return instances[index];
}
template<typename EventType>
void App::Input::SignalInternal::EventSlot<EventType>::clear() {
	instances.clear();
}
template<typename EventType>
void App::Input::SignalInternal::EventSlot<EventType>::reserve_slots(size_t pCount) {
	allSignals.reserve(allSignals.size() + pCount);
	instances.reserve(instances.size() + pCount);
}
template<typename R, typename... Args>
void App::Input::SignalInternal::FunctorImpl<R, Args...>::clear() {
	instances.clear();
}
template<typename R, typename... Args>
void App::Input::SignalInternal::FunctorImpl<R, Args...>::reserve_slots(size_t pCount) {
	instances.reserve(pCount);
}
template<typename R, typename... Args>
void App::Input::SignalInternal::FunctorImpl<R, Args...>::listen(FunctorImpl<R, Args...>& pSlot, std::initializer_list<size_t> pSignals)
{
	pSlot.signalBindings.insert(pSlot.signalBindings.end(), pSignals.begin(), pSignals.end());
}
template<typename R, typename... Args>
R App::Input::SignalInternal::FunctorImpl<R, Args...>::invoke() const {
	return std::apply(func, args);
}
template<typename R, typename... Args>
void App::Input::Functor<R, Args...>::listen(std::initializer_list<size_t> pSignals) {
	SignalInternal::FunctorImpl<R, Args...>::listen(SignalInternal::FunctorImpl<R, Args...>::instances[slotIndex], pSignals);
}


