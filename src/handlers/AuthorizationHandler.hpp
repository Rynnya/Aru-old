#ifndef HimitsuAuthorizationHandler_hpp
#define HimitsuAuthorizationHandler_hpp

#include "Globals.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"

class TokenObject : public oatpp::web::server::handler::AuthorizationObject 
{
public:

    TokenObject(const oatpp::Boolean& _valid, const oatpp::Int32& user)
        : valid(_valid), userID(user)
    {}

    oatpp::Boolean valid;
    oatpp::Int32 userID;

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
            auto db = himitsu::ConnectionPool::getInstance()->getConnection();
            tokens token_table{};
            auto query = (*db)->prepare(sqlpp::select(token_table.user)
                .from(token_table)
                .where(token_table.token == sqlpp::parameter(token_table.token)));
            query.params.token = token->c_str();
            auto result = (**db)(query);

            if (result.empty())
                return std::make_shared<TokenObject>(false, 0);

            const auto& res = result.front();
            return std::make_shared<TokenObject>(true, res.user.value());
        }

        return std::make_shared<TokenObject>(false, 0);
    }

};

#endif