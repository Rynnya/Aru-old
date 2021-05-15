#ifndef HimitsuUtils_hpp
#define HimitsuUtils_hpp

#include "Globals.hpp"

namespace himitsu
{
	class utils
	{
	public:
		static std::string str_tolower(std::string str)
		{
			std::string res;
			for (unsigned char c : str) { res += std::tolower(c); }
			return res;
		}

		static bool intToBoolean(int i)
		{
			if (i == 1) return true;
			return false;
		}
	};
}

#endif