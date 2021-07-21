#ifndef handlers_AuthorizationHandler_hpp_included
#define handlers_AuthorizationHandler_hpp_included

#include "Globals.hpp"
#include <oatpp/core/macro/component.hpp>

#include "../database/tables/OtherTable.hpp"

class TokenObject
{
public:

    TokenObject(const bool& valid, const std::string& token, const int32_t& user, const int64_t& priv)
        : valid(valid)
        , token(token)
        , userID(user)
        , privileges(priv)
    {}

    bool valid;
    std::string token;
    int32_t userID;
    int64_t privileges;

};

class TokenAuthorizationHandler
{
public:

    TokenAuthorizationHandler() {}

    std::shared_ptr<TokenObject> handleAuthorization(const aru::Connection& db, const oatpp::String& token)
    {
        if (token)
        {
            const tables::tokens tokens_table{};
            auto result = db(sqlpp::select(tokens_table.user, tokens_table.privileges, tokens_table.token)
                .from(tokens_table)
                .where(tokens_table.token == token->c_str()
                    and tokens_table.last_updated + 1209600 /* 2 weeks */ > aru::time_convert::getEpochNow()));

            if (result.empty())
                return std::make_shared<TokenObject>(false, "", 0, 0);

            const auto& res = result.front();
            return std::make_shared<TokenObject>(true, res.token, res.user, res.privileges);
        }

        return std::make_shared<TokenObject>(false, "", 0, 0);
    }

};

#endif