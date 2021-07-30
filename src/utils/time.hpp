#ifndef utils_time_hpp_included
#define utils_time_hpp_included

#include "Globals.hpp"
#include <chrono>
#include "date/date.h"

namespace aru
{
	class time_convert
	{
	public:
		static int64_t getEpochNow()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		static int64_t getTimestamp(std::string date)
		{
			std::istringstream in(date);
			date::sys_seconds tp;
			in >> date::parse("%Y-%m-%d %H:%M:%S", tp);
			return tp.time_since_epoch().count();
		}

		static std::string getDate(int64_t timestamp)
		{
			date::sys_seconds time{ std::chrono::seconds(timestamp) };
			return date::format("%Y-%m-%dT%H:%M:%SZ", time);
		}
	};
}

#endif