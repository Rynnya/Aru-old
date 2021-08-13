#include "time.hpp"

#include <chrono>
#include "date/date.h"

int64_t aru::time_convert::get_epoch_now()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t aru::time_convert::get_timestamp(const std::string& date)
{
	std::istringstream in(date);
	date::sys_seconds tp;
	in >> date::parse("%Y-%m-%d %H:%M:%S", tp);
	return tp.time_since_epoch().count();
}

const std::string aru::time_convert::get_date(int64_t timestamp)
{
	date::sys_seconds time{ std::chrono::seconds(timestamp) };
	return date::format("%Y-%m-%dT%H:%M:%SZ", time);
}