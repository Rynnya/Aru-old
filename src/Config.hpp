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
		extern std::string host;
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

	extern std::string avatar_folder;

	extern std::vector<std::string> forbidden_nicknames;

	void parse();

}

#endif
