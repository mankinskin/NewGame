#pragma once
#include <algorithm>
#include <vector>
#include "Delta.h"
namespace gl {
	namespace GUI {
		
		template<typename T>
		struct MinPolicy {
			static T compare(T& lhs, T& rhs) {
				return std::min(lhs, rhs);
			}
		};
		template<typename T>
		struct MaxPolicy {
			static T compare(T& lhs, T& rhs) {
				return std::max(lhs, rhs);
			}
		};
		template<typename T, template<typename> class PassPolicy = MaxPolicy>
		struct Boundary {
			Boundary(T& pTargetVar, T& pCompareVar)
				:target(Source<T>(pTargetVar)), compare(Source<T>(pCompareVar)) {}
			Source<T> target;
			Source<T> compare;
			void limit() {
				target.set(PassPolicy<T>::compare(target.get(), compare.get()));
			}
		};

		template<typename T, template<typename> class PassPolicy = MaxPolicy>
		std::vector < Boundary<T, PassPolicy>> boundaries = std::vector<Boundary<T, PassPolicy>>();
		
		template<typename T, template<typename> class PassPolicy = MaxPolicy>
		void createBoundary(T& pTargetVar, T& pCompareVar) {
			boundaries<T, PassPolicy>.push_back(Boundary<T, PassPolicy>(pTargetVar, pCompareVar));
		}

		template<typename T, template<typename> class PassPolicy = MaxPolicy>
		void updateBoundaries() {
			for (Boundary<T, PassPolicy> bou : boundaries<T, PassPolicy>) {
				bou.limit();
			}
		}
	}
}