#ifndef handlers_AuthorizationHandler_hpp_included
#define handlers_AuthorizationHandler_hpp_included

#include "Globals.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"

class TokenObject : public oatpp::web::server::handler::AuthorizationObject 
{
public:

    TokenObject(
        const oatpp::Boolean& _valid,
        const oatpp::String& t,
        const oatpp::Int32& user,
        const oatpp::Int64& priv
    )
        : valid(_valid)
        , token(t)
        , userID(user)
        , privileges(priv)
    {}

    oatpp::Boolean valid;
    oatpp::String token;
    oatpp::Int32 userID;
    oatpp::Int64 privileges;

};

class TokenAuthorizationHandler : public oatpp::web::server::handler::AuthorizationHandler
{
public:

    TokenAuthorizationHandler()
        : AuthorizationHandler("Authorization", "tokenAuth")
    {}

    std::shared_ptr<AuthorizationObject> handleAuthorization(const oatpp::String& token) override
    {
        if (token)
        {
            auto db(aru::ConnectionPool::getInstance()->getConnection());
            const tables::tokens tokens_table{};
            auto query = db->prepare(sqlpp::select(tokens_table.user, tokens_table.privileges, tokens_table.token)
                .from(tokens_table)
                .where(tokens_table.token == sqlpp::parameter(tokens_table.token)));
            query.params.token = token->c_str();
            auto result = (*db)(query);

            if (result.empty())
                return std::make_shared<TokenObject>(false, "", 0, 0);

            const auto& res = result.front();
            return std::make_shared<TokenObject>(true, res.token.value().c_str(), res.user.value(), res.privileges.value());
        }

        return std::make_shared<TokenObject>(false, "", 0, 0);
    }

};

#endif