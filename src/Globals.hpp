#ifndef globals_hpp_included
#define globals_hpp_included

#define NOMINMAX // macros are evil

#include <iostream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

#include "sqlpp11/sqlpp11.h"
#include "sqlpp11/mysql/mysql.h"
#include "database/sql.hpp"
#include "cpp_redis/core/client.hpp"

using json = nlohmann::ordered_json;

#include "config.hpp"

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
	inline oatpp::String create_error(int32_t status, const std::string& message)
	{
		json response;
		response["error"]["code"] = status;
		response["error"]["message"] = message;
		std::string dumped = response.dump();
		return oatpp::String(dumped.c_str(), dumped.size(), true);
	}

	inline oatpp::String create_error(const oatpp::web::protocol::http::Status& status, const std::string& message)
	{
		return create_error(status.code, message);
	}
}

#endif