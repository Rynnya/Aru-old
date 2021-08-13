#ifndef utils_time_hpp_included
#define utils_time_hpp_included

#include "globals.hpp"

namespace aru
{
	class time_convert
	{
	public:
		static int64_t get_epoch_now();
		static int64_t get_timestamp(const std::string& date);
		static const std::string get_date(int64_t timestamp);
	};
}

#endif