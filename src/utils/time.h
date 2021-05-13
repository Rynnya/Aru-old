#ifndef HIMITSU_TIME
#define HIMITSU_TIME

#include "Globals.h"
#include <chrono>
#include <iomanip>

namespace himitsu
{
	class time
	{
	public:
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
			struct tm  tstruct;
			char       buf[80];
			gmtime_s(&tstruct, &now);
			strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tstruct);

			return buf;
		}
	};
}

#endif // !HIMITSU_TIME