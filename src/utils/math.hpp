#ifndef HimitsuMath_hpp
#define HimitsuMath_hpp

#include "Globals.hpp"

namespace himitsu
{
	class math
	{
	public:
		template<typename T>
		static bool is_infinite(const T& value)
		{
			T max_value = std::numeric_limits<T>::max();
			T min_value = -max_value;

			return !(min_value <= value && value <= max_value);
		}

		template<typename T>
		static bool is_nan(const T& value)
		{
			// True if NAN
			return value != value;
		}
	};
}

#endif