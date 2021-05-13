#ifndef HIMITSU_GLOBALS
#define HIMITSU_GLOBALS

#define NOMINMAX // macros are evil

#include <iostream>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/connection_pool.h>
#include "database/SQL.h"
#include "database/connection/HimitsuDB.h"
#include "database/connection/Redis.h"

#include "Config.h"

#include "utils/curl.h"
#include "utils/math.h"
#include "utils/osu.h"
#include "utils/utils.h"
#include "utils/time.h"

namespace mysql = sqlpp::mysql;
using json = nlohmann::json;

#define CORS_SUPPORT(RESPONSE) \
    do { \
        RESPONSE->putHeaderIfNotExists(oatpp::String("Access-Control-Allow-Methods"), oatpp::String("GET, POST, PUT, OPTIONS, DELETE"));  \
        RESPONSE->putHeaderIfNotExists(oatpp::String("Access-Control-Allow-Origin"), oatpp::String("*")); \
        RESPONSE->putHeaderIfNotExists(oatpp::String("Access-Control-Max-Age"), oatpp::String("1728000")); \
    } while(false);

#endif // !HIMITSU_GLOBALS