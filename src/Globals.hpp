#ifndef HimitsuGlobals_hpp
#define HimitsuGlobals_hpp

#define NOMINMAX // macros are evil

#include <iostream>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/connection_pool.h>
#include "database/SQL.hpp"
#include "database/connection/HimitsuDB.hpp"
#include "database/connection/Redis.hpp"

#include "Config.hpp"

#include "utils/curl.hpp"
#include "utils/math.hpp"
#include "utils/osu.hpp"
#include "utils/time.hpp"
#include "utils/utils.hpp"

namespace mysql = sqlpp::mysql;
using json = nlohmann::json;

#endif