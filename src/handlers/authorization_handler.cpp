#include "authorization_handler.hpp"

#include "../database/tables/other_table.hpp"

const std::shared_ptr<token_object> token_authorization_handler::handle_authorization(const aru::database& db, const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request)
{
    const oatpp::String& token = request->getHeader("Authorization");

    // If external request
    if (token && token->getSize() != 0)
    {
        return authorize_through_token(db, token);
    }

    const oatpp::String& cookie = request->getHeader("Cookie");

    // If internal request from browser
    if (cookie && cookie->getSize() != 0)
    {
        return authorize_through_cookie(db, cookie);
    }

    // Unauthorized
    return std::make_shared<token_object>(false, "", 0, 0);
}

const std::shared_ptr<token_object> token_authorization_handler::authorize_through_token(const aru::database& db, const oatpp::String& token)
{
    const tables::tokens tokens_table{};
    auto result = db(
        sqlpp::select(tokens_table.user, tokens_table.privileges, tokens_table.token)
        .from(tokens_table)
        .where(
            tokens_table.token == token->c_str() and
            tokens_table.last_updated + 1209600 /* 2 weeks */ > aru::time_convert::get_epoch_now()
        ));

    if (result.empty())
    {
        return std::make_shared<token_object>(false, "", 0, 0);
    }

    const auto& res = result.front();
    return std::make_shared<token_object>(true, res.token, res.user, res.privileges);
}

const std::shared_ptr<token_object> token_authorization_handler::authorize_through_cookie(const aru::database& db, const oatpp::String& _cookie)
{
    const std::unordered_map<std::string_view, std::string_view>& cookies = aru::utils::parse_cookie_string(_cookie);

    auto cookie = cookies.find("hat");
    if (cookie == cookies.end())
    {
        return std::make_shared<token_object>(false, "", 0, 0);
    }

    const tables::tokens tokens_table{};
    auto result = db(
        sqlpp::select(tokens_table.user, tokens_table.privileges, tokens_table.token)
        .from(tokens_table)
        .where(
            tokens_table.token == cookie->second.data() and
            tokens_table.last_updated + 1209600 /* 2 weeks */ > aru::time_convert::get_epoch_now()
        ));

    if (result.empty())
    {
        return std::make_shared<token_object>(false, "", 0, 0);
    }

    const auto& res = result.front();
    return std::make_shared<token_object>(true, res.token, res.user, res.privileges);
}