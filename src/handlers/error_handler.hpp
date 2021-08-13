#ifndef handlers_error_handler_hpp_included
#define handlers_error_handler_hpp_included

#include "globals.hpp"
#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

class json_error_handler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler
{
public:
    json_error_handler() {}

    static std::shared_ptr<json_error_handler> create_shared()
    {
        return std::make_shared<json_error_handler>();
    }

    // Other type of writing because we override this function -> handle_error
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> handleError(
        const oatpp::web::protocol::http::Status& status,
        const oatpp::String& message,
        const Headers& headers
    ) override
    {
        using namespace oatpp::web::protocol;
        auto response = http::outgoing::Response::createShared(
            status,
            http::outgoing::BufferBody::createShared(aru::create_error(status, message->std_str()))
        );

        response->putHeader(http::Header::SERVER, http::Header::Value::SERVER);
        response->putHeader(http::Header::CONNECTION, http::Header::Value::CONNECTION_CLOSE);
        response->putHeaderIfNotExists("Content-Type", "application/json");

        for (const auto& pair : headers.getAll())
        {
            response->putHeader_Unsafe(pair.first, pair.second);
        }

        return response;
    };

};

#endif