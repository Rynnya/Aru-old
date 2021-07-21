#ifndef Globals_hpp_included
#define Globals_hpp_included

#define NOMINMAX // macros are evil

#include <iostream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

#include "sqlpp11/sqlpp11.h"
#include "sqlpp11/mysql/mysql.h"
#include "database/SQL.hpp"
#include "cpp_redis/core/client.hpp"

using json = nlohmann::ordered_json;

#include "Config.hpp"

#include "utils/convert.hpp"
#include "utils/curl.hpp"
#include "utils/math.hpp"
#include "utils/osu.hpp"
#include "utils/time.hpp"
#include "utils/utils.hpp"

#include "oatpp/web/protocol/http/Http.hpp"

namespace mysql = sqlpp::mysql;

namespace aru
{
	inline oatpp::String createError(int32_t status, std::string message)
	{
		json response;
		response["error"]["code"] = status;
		response["error"]["message"] = message;
		std::string dumped = response.dump();
		return oatpp::String(dumped.c_str(), dumped.size(), true);
	}

	inline oatpp::String createError(const oatpp::web::protocol::http::Status& status, std::string message)
	{
		return createError(status.code, message);
	}
}

#endif