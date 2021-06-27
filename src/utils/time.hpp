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
		static long long getEpochNow()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		static long long getTimestamp(std::string date)
		{
			std::istringstream in(date);
			date::sys_seconds tp;
			in >> date::parse("%Y-%m-%d %H:%M:%S", tp);
			return tp.time_since_epoch().count(); // TODO: Make UTC instead of Local time (Currently not critical)
		}

		static std::string getDate(long long timestamp)
		{
			date::sys_seconds time{ std::chrono::seconds(timestamp) };
			return date::format("%Y-%m-%dT%H:%M:%SZ", time);
		}
	};
}

#endif