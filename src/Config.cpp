#include "Config.hpp"

// Default config
namespace config
{
	namespace database
	{
		int32_t     connection_amount = 10;
		std::string host              = "localhost";
		int32_t     port              = 3306;
		std::string username          = "root";
		std::string password          = "";
		std::string database          = "yukime";
		bool        debug             = false; // Set True if want to see all sqlpp requests
	}

	namespace redis
	{
		std::string address               = "127.0.0.1";
		size_t      port                  = 6379;
		std::string password              = "";
	}

	namespace limits
	{
		bool enable_rate_limit = true;
		uint32_t limit = 2000;
		uint32_t timer = 60;
	}

	std::string frontend_site = "yukime.ml";
	std::string avatar_folder = "/path/to/folder/{}.png"; // {} -> fmt::format(id)

	// Before adding nickname to this vector, please make it safe
	// Replace all spaces with underscores and all capital letters with small ones
	// Example: 'My Super Nick' => 'my_super_nick'
	std::vector<std::string> forbidden_nicknames =
	{
		"peppy", "rrtyui", "cookiezi", "azer", "loctav", "banchobot", "happystick", "doomsday", "sharingan33", "andrea", "cptnxn", "reimu-desu",
		"hvick225", "_index", "my_aim_sucks", "kynan", "rafis", "sayonara-bye", "thelewa", "wubwoofwolf", "millhioref", "tom94", "tillerino", "clsw",
		"spectator", "exgon", "axarious", "angelsim", "recia", "nara", "emperorpenguin83", "bikko", "xilver", "vettel", "kuu01", "_yu68", "tasuke912", "dusk",
		"ttobas", "velperk", "jakads", "jhlee0133", "abcdullah", "yuko-", "entozer", "hdhr", "ekoro", "snowwhite", "osuplayer111", "musty", "nero", "elysion",
		"ztrot", "koreapenguin", "fort", "asphyxia", "niko", "shigetora", "whitecat", "fokabot", "himitsu", "nebula", "howl", "nyo", "angelwar", "mm00", "yukime"
	};

	void parse()
	{
		std::fstream cfg_file("config.json");

		if (!cfg_file)
		{
			fmt::print("Config: Cannot read config.json... Trying to use default one.\n");
			fmt::print("Config: This might cause exception by sqlpp or cpp-redis if password is none.\n");
			return;
		}

		std::stringstream strStream;
		strStream << cfg_file.rdbuf();
		json json_config = json::parse(strStream, nullptr, false);
		cfg_file.close();

		if (json_config.is_discarded())
		{
			fmt::print("Config: config.json is invalid... Trying to use default one.\n");
			fmt::print("Config: This might cause exception by sqlpp or cpp-redis if password is none.\n");
			return;
		}

		// Unreadable shit incoming!
		if (json_config["database"]["connection_amount"].is_number_integer())
			database::connection_amount = json_config["database"]["connection_amount"];
		if (json_config["database"]["host"].is_string())
			database::host = json_config["database"]["host"];
		if (json_config["database"]["port"].is_number_integer())
			database::port = json_config["database"]["port"];
		if (json_config["database"]["username"].is_string())
			database::username = json_config["database"]["username"];
		if (json_config["database"]["password"].is_string())
			database::password = json_config["database"]["password"];
		if (json_config["database"]["database"].is_string())
			database::database = json_config["database"]["database"];
		if (json_config["database"]["debug"].is_boolean())
			database::debug = json_config["database"]["debug"];

		if (json_config["redis"]["address"].is_string())
			redis::address = json_config["redis"]["address"];
		if (json_config["redis"]["port"].is_number_unsigned())
			redis::port = json_config["redis"]["port"];
		if (json_config["redis"]["password"].is_string())
			redis::password = json_config["redis"]["password"];

		if (json_config["limits"]["enable_rate_limit"].is_boolean())
			limits::enable_rate_limit = json_config["limits"]["enable_rate_limit"];
		if (json_config["limits"]["limit"].is_number_unsigned())
			limits::limit = json_config["limits"]["limit"];
		if (json_config["limits"]["timer"].is_number_unsigned())
			limits::timer = json_config["limits"]["timer"];

		if (json_config["frontend_site"].is_string())
			frontend_site = json_config["frontend_site"];
		if (json_config["avatar_folder"].is_string())
			avatar_folder = json_config["avatar_folder"];

		fmt::print("Config: Successfully parsed! Starting...\n");
	};

}
