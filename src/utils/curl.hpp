#ifndef utils_curl_hpp_included
#define utils_curl_hpp_included

#include "globals.hpp"
#include <curl/curl.h>
#include <tuple>

namespace aru
{
	class curl
	{
	private:
		static size_t internal_callback(void* raw_data, size_t size, size_t memory, std::string* ptr);
	public:
		static std::tuple<bool, std::string> get(const std::string& url);
	};
}

#endif