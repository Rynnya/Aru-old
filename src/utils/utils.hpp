#ifndef utils_utils_hpp_included
#define utils_utils_hpp_included

#include "globals.hpp"

#include <oatpp/web/server/HttpRequestHandler.hpp>

namespace aru
{
	class utils
	{
	public:
		// Helpers for oatpp::Request
		static std::pair<std::string_view, std::string_view> split_pair(const char* ptr, size_t size, char delim);
		static const std::unordered_map<std::string_view, std::string_view> parse_cookie_string(const oatpp::String& _cookie);
		static const std::string get_ip_address(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request);

		// Trimmers
		static std::string rtrim(std::string& s, const char* t = " \t\n\r\f\v");
		static std::string ltrim(std::string& s, const char* t = " \t\n\r\f\v");
		static std::string trim(std::string& s, const char* t = " \t\n\r\f\v");

		// Randoms
		static uint32_t gen_random_number();
		static std::string gen_random_string(const int32_t& len);

		// toLower for std::string
		static void str_tolower(std::string& str);
		static std::string str_tolower(const std::string& str);

		static bool int_to_bool(const int32_t& i, bool more = false);

		inline static void sanitize(int32_t& value, int32_t min, int32_t max, int32_t _default = 0)
		{
			value = value > max ? _default : value < min ? _default : value;
		}
	};
}

#endif
