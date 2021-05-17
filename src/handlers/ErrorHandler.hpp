#ifndef HimitsuErrorHandler_hpp
#define HimitsuErrorHandler_hpp

#include "Globals.hpp"
#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

class JsonErrorHandler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler {
public:
    JsonErrorHandler() {}

    static std::shared_ptr<JsonErrorHandler> createShared() {
        return std::make_shared<JsonErrorHandler>();
    }

    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>
        handleError(const oatpp::web::protocol::http::Status& status, const oatpp::String& message, const Headers& headers) override
    {
        auto response = oatpp::web::protocol::http::outgoing::Response::createShared
        (status, oatpp::web::protocol::http::outgoing::BufferBody::createShared(himitsu::createError(status, message->c_str()).c_str()));

        response->putHeader(oatpp::web::protocol::http::Header::SERVER, oatpp::web::protocol::http::Header::Value::SERVER);
        response->putHeader(oatpp::web::protocol::http::Header::CONNECTION, oatpp::web::protocol::http::Header::Value::CONNECTION_CLOSE);
        response->putHeaderIfNotExists("Content-Type", "application/json");

        for (const auto& pair : headers.getAll()) {
            response->putHeader_Unsafe(pair.first, pair.second);
        }

        return response;
    };

};

#endif