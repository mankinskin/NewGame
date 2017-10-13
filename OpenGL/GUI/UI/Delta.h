#pragma once
#include <vector>
namespace gl {
	namespace GUI {
		//Sources can get values from anywhere in the program
		template<typename T>
		struct Source {
			Source(T& pSource)
				:source(pSource) {}
			T& get() {
				return source;
			}
			void set(T newVal) {
				source = newVal;
			}
		private:
			T& source;
		};
		template<typename T>
		std::vector<Source<T>> sources = std::vector<Source<T>>();

		template<typename T>
		size_t createSource(T& pSourceValue) {
			sources<T>.push_back(Source<T>(pSourceValue));
			return sources<T>.size() - 1;
		}


		template<typename T>
		struct AddPolicy {
			static void apply(T& lhs, T& rhs) {
				lhs += rhs;
			}
		};
		template<typename T>
		struct SubstractPolicy {
			static void apply(T& lhs, T& rhs) {
				lhs -= rhs;
			}
		};
		template<typename T>
		struct MultiplyPolicy {
			static void apply(T& lhs, T& rhs) {
				lhs *= rhs;
			}
		};
		template<typename T>
		struct DividePolicy {
			static void apply(T& lhs, T& rhs) {
				lhs /= rhs;
			}
		};
		template<typename T>
		struct EqualsPolicy {
			static void apply(T& lhs, T& rhs) {
				lhs = rhs;
			}
		};
		template<typename T, template<typename> class Operator = AddPolicy>
		struct DeltaTarget{
			DeltaTarget(T& pTarget, size_t pSourceID = -1)
				:target(pTarget), sourceID(pSourceID), active(false) {}
			void applySource() {
				if(active)
					Operator<T>::apply(target, sources<T>[sourceID].get());
			}
			void setSource(size_t pSourceID) {
				
				sourceID = pSourceID;
			}
			void activate() {
				active = true;
			}
			void deactivate() {
				active = false;
			}
			void toggle() {
				active = !active;
			}
		private:
			size_t sourceID;
			bool active;
			T& target;
		};
		template<typename T, template<typename> class Operator = AddPolicy>
		std::vector<DeltaTarget<T, Operator>> targets = std::vector<DeltaTarget<T, Operator>>();

		
		
		template<typename T, template<typename> class Operator = AddPolicy>
		size_t createTarget(T & pTargetValue, size_t pSourceID = -1) {
			targets<T, Operator>.push_back(DeltaTarget<T, Operator>(pTargetValue, pSourceID));
			return targets<T, Operator>.size() - 1;
		}
		template<typename T, template<typename> class Operator = AddPolicy>
		void updateDeltas() {
			for (DeltaTarget<T, Operator> tar : targets<T, Operator>) {
				tar.applySource();
			}
		}
		template<typename T, template<typename> class Operator = AddPolicy>
		void toggleTarget(size_t pTargetID) {
			targets<T, Operator>[pTargetID].toggle();
		}
		template<typename T, template<typename> class Operator = AddPolicy>
		void activateTarget(size_t pTargetID) {
			targets<T, Operator>[pTargetID].activate();
		}
		template<typename T, template<typename> class Operator = AddPolicy>
		void deactivateTarget(size_t pTargetID) {
			targets<T, Operator>[pTargetID].deactivate();
		}
	}
}