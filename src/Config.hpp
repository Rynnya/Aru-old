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

	// Forbidden Nicknames // Before add nickname there - make it safe (Replace spaces with '_', toLower)
	static std::vector<std::string> forbidden_nicknames = { "peppy", "rrtyui", "cookiezi", "azer", "loctav", "banchobot", "happystick", "doomsday", "sharingan33", "andrea", "cptnxn",
		"reimu-desu", "hvick225", "_index", "my_aim_sucks", "kynan", "rafis", "sayonara-bye", "thelewa", "wubwoofwolf", "millhioref", "tom94", "tillerino", "clsw",
		"spectator", "exgon", "axarious", "angelsim", "recia", "nara", "emperorpenguin83", "bikko", "xilver", "vettel", "kuu01", "_yu68", "tasuke912", "dusk",
		"ttobas", "velperk", "jakads", "jhlee0133", "abcdullah", "yuko-", "entozer", "hdhr", "ekoro", "snowwhite", "osuplayer111", "musty", "nero", "elysion",
		"ztrot", "koreapenguin", "fort", "asphyxia", "niko", "shigetora", "whitecat", "fokabot", "himitsu", "nebula", "howl", "nyo", "angelwar", "mm00" };
}

#endif
