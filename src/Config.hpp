#ifndef config_hpp_included
#define config_hpp_included

#include "globals.hpp"
#include <sstream>
#include <fstream>

// Config table, to change default values see Config.cpp
namespace config
{
	namespace database
	{
		extern int32_t connection_amount;
		extern std::string host;
		extern int32_t port;
		extern std::string username;
		extern std::string password;
		extern std::string database;
		extern bool debug;
	}

	namespace redis
	{
		extern std::string address;
		extern size_t port;
		extern std::string password;
	}

	namespace limits
	{
		extern bool enable_rate_limit;
		extern uint32_t limit;
		extern uint32_t timer;
	}

	extern std::string frontend_site;
	extern std::string avatar_folder;

	extern std::vector<std::string> forbidden_nicknames;

	void parse();

}

#endif
