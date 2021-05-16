#ifndef HimitsuConfig_hpp
#define HimitsuConfig_hpp

namespace config
{
	// Database
	static int         db_connection_amount = 10;
	static int         db_port     = 3307; // Default port for MySQL/MariaDB
	static std::string db_user     = "root";
	static std::string db_password = "";
	static std::string db_database = "himitsu";
	static bool        db_debug    = false; // Set True if you need debug output in sqlpp

	// Redis
	static std::string redis_address               = "127.0.0.1";
	static size_t      redis_port                  = 6379;
	static uint32_t    redis_reconnection_attempts = 3;
	static std::string redis_password              = "";

	// osu!API
	static bool        api_enabled = true; // Set False if you don't want to download full beatmap set when LETS request it
	static std::string api_key     = ""; // Get key there: https://osu.ppy.sh/p/api/

	// Folders
	static std::string avatar_folder = "/path/to/folder/{}.png"; // {} -> fmt::format(id) 
}

#endif
