#ifndef handlers_authorization_handler_hpp_included
#define handlers_authorization_handler_hpp_included

#include "globals.hpp"
#include <oatpp/core/macro/component.hpp>
#include <oatpp/web/protocol/http/outgoing/Request.hpp>

class token_object
{
public:

    token_object(const bool& valid, const std::string& token, const int32_t& user, const int64_t& priv)
        : valid(valid)
        , token(token)
        , user_id(user)
        , privileges(priv)
    {}

    const bool valid;
    const std::string token;
    const int32_t user_id;
    const int64_t privileges;

};

class token_authorization_handler
{
public:

    token_authorization_handler() {}

    const std::shared_ptr<token_object> handle_authorization(
        const aru::database& db,
        const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request
    );

private:
    const std::shared_ptr<token_object> authorize_through_token(const aru::database& db, const oatpp::String& token);
    const std::shared_ptr<token_object> authorize_through_cookie(const aru::database& db, const oatpp::String& _cookie);
};

#endif