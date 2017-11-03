#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <tuple>
/*
Each functor holds a function pointer and a tuple for the arguments.
when a functor class is first instantited from the template, its static call(size_t) function is given to the functorInvokers.
this function will later be used to call a functor object by its index.
when a functor object is created, its template_index and its slot index is inserted into the order_queue, which ensures that all functors will always be called in the order in which they were created
when checking all functors, the functorOrder queue is used. the invoker_index is used to determine the class of the functor and the slot_index is used to find the object to be invoked.
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
			template<class SignalGate, typename... Args>
			class FunctorImpl {
			public:
				FunctorImpl(void(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
					:func(pF), args(pArgs...), signalBindings(pSignals)
				{}
				static void clear();
				static void reserve_slots(size_t pCount);
				static void call(size_t index);
				void listen(std::initializer_list<size_t> pSignals);

				void invoke() const;
				static std::vector<FunctorImpl<SignalGate, Args...>> instances;
				std::vector<size_t> signalBindings;
			private:
				void(*func)(Args...);
				std::tuple<Args...> args;
			};
			bool is_on(size_t pSignalIndex);


			template<class SignalGate, typename... Args>
			std::vector<FunctorImpl<SignalGate, Args...>> FunctorImpl<SignalGate, Args...>::instances = std::vector<FunctorImpl<SignalGate, Args...>>();

			extern std::vector<void(*)(size_t)> functorInvokers;//invokes all functor templates
			extern std::vector<void(*)()> functorDestructors;//destroys all functor templates
			extern std::vector<std::pair<size_t, size_t>> functorOrder;

		}//end Internal

		//
		//
		template<class SignalGate, typename... Args>
		struct Functor {//Handle
			Functor(void(*pF)(Args...), Args... pArgs);
			Functor(void(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals);
			void listen(std::initializer_list<size_t> pSignals);
			size_t slot_index;
			static size_t invoker_index;
		private:
			void createFunctor(void(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals = std::initializer_list<size_t>());
		};

		void callFunctors();
		void clearFunctors();
		template<class SignalGate, typename... Args>
		size_t Functor<SignalGate, Args...>::invoker_index = 0;
		template<class SignalGate, typename... Args>
		void reserve_functors(size_t pCount) {
			FunctorInternal::FunctorImpl<SignalGate, Args...>::reserve_slots(pCount);
		}
	}//end Input
}//end App

template<class SignalGate, typename... Args>
App::Input::Functor<SignalGate, Args...>::Functor(void(*pF)(Args...), Args... pArgs)
	:slot_index(App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., std::initializer_list<size_t>());
}
template<class SignalGate, typename... Args>
App::Input::Functor<SignalGate, Args...>::Functor(void(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
	: slot_index(App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., pSignals);
}

template<class SignalGate, typename... Args>
void App::Input::Functor<SignalGate, Args...>::createFunctor(void(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
{
	using namespace FunctorInternal;
	if (slot_index == 0) {
		invoker_index = functorInvokers.size();
		functorDestructors.push_back(FunctorImpl<SignalGate, Args...>::clear);
		functorInvokers.push_back(FunctorImpl<SignalGate, Args...>::call);
	}
	FunctorImpl<SignalGate, Args...>::instances.emplace_back(pF, pArgs..., pSignals);
	functorOrder.emplace_back(Functor<SignalGate, Args...>::invoker_index, slot_index);
}
template<class SignalGate, typename... Args>
void App::Input::Functor<SignalGate, Args...>::listen(std::initializer_list<size_t> pSignals) {
	FunctorInternal::FunctorImpl<SignalGate, Args...>::instances[slotIndex].listen(pSignals);
}

template<class SignalGate, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::clear() {
	instances.clear();
}
template<class SignalGate, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::reserve_slots(size_t pCount) {
	instances.reserve(pCount);
}
template<class SignalGate, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::listen(std::initializer_list<size_t> pSignals)
{
	signalBindings.insert(signalBindings.end(), pSignals.begin(), pSignals.end());
}
template<class SignalGate, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::call(size_t index) {
	if (SignalGate::gate(FunctorImpl<SignalGate, Args...>::instances[index].signalBindings.begin(), FunctorImpl<SignalGate, Args...>::instances[index].signalBindings.end(), is_on)) {
		FunctorImpl<SignalGate, Args...>::instances[index].invoke();
	}
}
template<class SignalGate, typename... Args>
void App::Input::FunctorInternal::FunctorImpl<SignalGate, Args...>::invoke() const {
	std::apply(func, args);
}



//ANOTHER APPROACH
// EXPRESSION FUNCTORS
//
//template<typename T>
//struct Min {
//	Min(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T& operator()() {
//		return lhs < rhs ? lhs : rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs < pRhs ? pLhs : pRhs;
//		
//	}
//	
//		
//};
//template<typename T>
//struct Max {
//	Max(T& LHS, T& RHS)
//		:lhs(LHS), rhs(RHS) {}
//	
//		
//		T& lhs;
//	T& rhs;
//	inline T& operator()() {
//		return lhs > rhs ? lhs : rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs > pRhs ? pLhs : pRhs;
//		
//	}
//};
//template<typename T>
//struct Add {
//	Add(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T operator()() {
//		return lhs + rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs + pRhs;
//		
//	}
//
//};
//template<typename T>
//struct Substract {
//	Substract(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T operator()() {
//		return lhs - rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs - pRhs;
//		
//	}
//};
//template<typename T>
//struct Pass {
//		Pass(T& VAL)
//		 :lhs(VAL) {}
//	
//		T& lhs;
//	inline T& operator()() {
//		return lhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pDummy) {
//		return pLhs;
//		
//	}
//};
//template<typename T>
//struct Set {
//	Set(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T& operator()() {
//		lhs = rhs;
//		return lhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pTarget, R&& pSource) {
//			pTarget = pSource;
//			return pTarget;
//	}
//};
//template<typename T, template<typename> class FuncPolicy, class LhsSource, class RhsSource>
//struct Operation {
//	Operation(LhsSource pLhsSource, RhsSource pRhsSource)
//		 :lhs(pLhsSource), rhs(pRhsSource) {
//		}
//	T operator()() {
//		return FuncPolicy<T>::func(lhs(), rhs());
//	}
//	LhsSource lhs;
//	RhsSource rhs;
//};