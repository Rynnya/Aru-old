#ifndef utils_utils_hpp_included
#define utils_utils_hpp_included

#include "Globals.hpp"
#include <random>

namespace aru
{
	class utils
	{
	public:
		// trim from end of string (right)
		static std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			s.erase(s.find_last_not_of(t) + 1);
			return s;
		}

		// trim from beginning of string (left)
		static std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			s.erase(0, s.find_first_not_of(t));
			return s;
		}

		// trim from both ends of string (right then left)
		static std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			return ltrim(rtrim(s, t), t);
		}

		static uint32_t genRandomInt()
		{
			static std::random_device rd;
			static std::mt19937 engine(rd());

			return engine();
		}

		static std::string genRandomString(const int32_t len)
		{

			std::string tmp_s;
			static const char alphanum[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";


			tmp_s.reserve(len);
			for (int32_t i = 0; i < len; ++i)
				tmp_s.push_back(alphanum[genRandomInt() % (sizeof(alphanum) - 1)]);

			return tmp_s;
		}

		static void str_tolower(std::string& str)
		{
			for (size_t i = 0; i < str.size(); i++)
			{
				str[i] = std::tolower(str[i]);
			}
		}

		static std::string str_tolower(const std::string& str)
		{
			std::string res = str;
			str_tolower(res);
			return res;
		}

		static bool intToBoolean(int32_t i, bool more = false)
		{
			if (i == 1)
				return true;

			if (more && i >= 1)
				return true;

			return false;
		}
	};
}

#endif
