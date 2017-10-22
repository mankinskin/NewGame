#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
/*
   ---

   ---Usage-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

   --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
namespace App {
	namespace Input {
		struct AnySignalGate {

			static bool gate(std::vector<size_t>::iterator pHeadIt, std::vector<size_t>::iterator pTailIt, bool(*check)(size_t)) {
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
				R(&func)(Args...);
				std::tuple<Args...> args;

			public:
				static std::vector<FunctorImpl<SignalGate, R, Args...>> instances;
				std::vector<size_t> signalBindings;

				FunctorImpl(R(&pF)(Args...), Args... pArgs)
					:func(pF), args(std::forward_as_tuple(pArgs...))
				{}
				FunctorImpl(R(&pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
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
			Functor(R(&pF)(Args...), Args... pArgs);
			Functor(R(&pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals);
			void listen(std::initializer_list<size_t> pSignals);
			size_t slotIndex;
		private:
			void createFunctor(R(&pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals = std::initializer_list<size_t>());
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
App::Input::Functor<SignalGate, R, Args...>::Functor(R(&pF)(Args...), Args... pArgs)
	:slotIndex(App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., std::initializer_list<size_t>());
}
template<class SignalGate, typename R, typename... Args>
App::Input::Functor<SignalGate, R, Args...>::Functor(R(&pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
	: slotIndex(App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., pSignals);
}

template<class SignalGate, typename R, typename... Args>
void App::Input::Functor<SignalGate, R, Args...>::createFunctor(R(&pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
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



