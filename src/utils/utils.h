#ifndef HIMITSU_UTILS
#define HIMITSU_UTILS

#include "Globals.h"

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