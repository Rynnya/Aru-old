#ifndef utils_hash_hpp_included
#define utils_hash_hpp_included

#include "globals.hpp"

namespace aru
{
	class hash
	{
	public:
		static std::string create_sha512(const std::string& input, const std::string& salt);
		static std::string create_md5(const std::string& input);
	};
}

#endif
