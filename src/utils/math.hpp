#ifndef utils_math_hpp_included
#define utils_math_hpp_included

#include "globals.hpp"

namespace aru
{
	class math
	{
	public:
		template<typename T>
		static bool is_infinite(const T& value)
		{
			static_assert(std::is_arithmetic<T>::value, "T must be arithmetic.");
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