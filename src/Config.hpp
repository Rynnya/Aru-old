#ifndef Config_hpp_included
#define Config_hpp_included

#include "Globals.hpp"
#include <sstream> 
#include <fstream>

// Config table, to change default values see Config.cpp
namespace config
{
	namespace database
	{
		extern int connection_amount;
		extern int port;
		extern std::string username;
		extern std::string password;
		extern std::string database;
		extern bool debug;
	}

	namespace redis
	{
		extern std::string address;
		extern size_t port;
		extern uint32_t reconnection_attempts;
		extern std::string password;
	}

	namespace osu_api
	{
		extern bool enabled;
		extern std::string osu_key;
		extern std::string bancho_key;
		extern int cooldown;
		extern bool currently_running;
	}

	extern std::string avatar_folder;

	extern std::vector<std::string> forbidden_nicknames;

	void parse();

}

#endif
