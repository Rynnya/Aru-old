#ifndef utils_time_hpp_included
#define utils_time_hpp_included

#include "Globals.hpp"
#include <chrono>
#include <iomanip>

namespace himitsu
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
			tm tm = {};
			std::stringstream ss(date);
			ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
			std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&tm));
			return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
		}

		static std::string getDate(long long timestamp) {
			std::chrono::time_point<std::chrono::system_clock> date = std::chrono::time_point<std::chrono::system_clock>{} + std::chrono::seconds(timestamp);
			time_t     now = std::chrono::system_clock::to_time_t(date);
			struct tm tstruct;
#if _MSC_VER && !__INTEL_COMPILER
			gmtime_s(&tstruct, &now);
#else
			gmtime_r(&now, &tstruct);
#endif
			std::ostringstream oss;
			oss << std::put_time(&tstruct, "%Y-%m-%dT%H:%M:%SZ");

			return oss.str();
		}
	};
}

#endif