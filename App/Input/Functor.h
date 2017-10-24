#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>


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
			public:
				FunctorImpl(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
					:func(pF), args(std::forward_as_tuple<Args...>(pArgs...)), signalBindings(pSignals)
				{}
				static void clear();
				static void reserve_slots(size_t pCount);

				void listen(std::initializer_list<size_t> pSignals);
				R invoke() const;

				static std::vector<FunctorImpl<SignalGate, R, Args...>> instances;
				std::vector<size_t> signalBindings;
			private:
				R(*func)(Args...);
				std::tuple<Args...> args;
			};
			bool is_on(size_t pSignalIndex);
			template<class SignalGate, typename R, typename... Args>
			void callFunctorInstances()//call each functor of a template if their 
			{
				for (auto& inst : FunctorImpl<SignalGate, R, Args...>::instances) {
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
	: slotIndex(App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances.size())
{
	createFunctor(pF, pArgs..., pSignals);
}

template<class SignalGate, typename R, typename... Args>
void App::Input::Functor<SignalGate, R, Args...>::createFunctor(R(*pF)(Args...), Args... pArgs, std::initializer_list<size_t> pSignals)
{
	using namespace FunctorInternal;
	if (FunctorImpl<SignalGate, R, Args...>::instances.size() == 0) {
		functorInvokers.push_back(callFunctorInstances<SignalGate, R, Args...>);
		functorDestructors.push_back(FunctorImpl<SignalGate, R, Args...>::clear);
	}
	FunctorImpl<SignalGate, R, Args...>::instances.emplace_back(pF, pArgs..., pSignals);

}
template<class SignalGate, typename R, typename... Args>
void App::Input::Functor<SignalGate, R, Args...>::listen(std::initializer_list<size_t> pSignals) {
	FunctorInternal::FunctorImpl<SignalGate, R, Args...>::instances[slotIndex].listen(pSignals);
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
void App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::listen(std::initializer_list<size_t> pSignals)
{
	signalBindings.insert(signalBindings.end(), pSignals.begin(), pSignals.end());
}
template<class SignalGate, typename R, typename... Args>
R App::Input::FunctorInternal::FunctorImpl<SignalGate, R, Args...>::invoke() const {
	return std::apply(func, args);
}



//ANOTHER APPROACH
// EXPRESSION FUNCTORS

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