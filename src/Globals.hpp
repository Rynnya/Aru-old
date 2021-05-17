#ifndef HimitsuGlobals_hpp
#define HimitsuGlobals_hpp

#define NOMINMAX // macros are evil

#include <iostream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

#include "sqlpp11/sqlpp11.h"
#include "sqlpp11/mysql/mysql.h"
#include "database/SQL.hpp"
#include "database/connection/HimitsuDB.hpp"
#include "database/connection/Redis.hpp"

#include "Config.hpp"

#include "utils/curl.hpp"
#include "utils/math.hpp"
#include "utils/osu.hpp"
#include "utils/time.hpp"
#include "utils/utils.hpp"

#include "oatpp/web/protocol/http/Http.hpp"

namespace mysql = sqlpp::mysql;
using json = nlohmann::json;

namespace himitsu
{
	inline std::string createError(const oatpp::web::protocol::http::Status& status, std::string message)
	{
		json response;
		response["error"]["code"] = status.code;
		response["error"]["message"] = message;
		return response.dump();
	}
}

#endif