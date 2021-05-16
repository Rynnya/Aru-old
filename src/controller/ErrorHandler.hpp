#ifndef ErrorHandler_hpp
#define ErrorHandler_hpp

#include "Globals.hpp"
#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include <oatpp/web/protocol/http/outgoing/BufferBody.hpp>

class JsonErrorHandler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler {
public:
    JsonErrorHandler() {}

    static std::shared_ptr<JsonErrorHandler> createShared() {
        return std::make_shared<JsonErrorHandler>();
    }

    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>
        handleError(const oatpp::web::protocol::http::Status& status, const oatpp::String& message, const Headers& headers) override
    {
        json answer;
        auto additions = message.get()->c_str();
        answer["statusCode"] = status.code;
        answer["message"] = additions;
        OATPP_LOGE("Error", additions);
        auto response = oatpp::web::protocol::http::outgoing::Response::createShared
        (status, oatpp::web::protocol::http::outgoing::BufferBody::createShared(answer.dump().c_str()));

        response->putHeader(oatpp::web::protocol::http::Header::SERVER, oatpp::web::protocol::http::Header::Value::SERVER);
        response->putHeader(oatpp::web::protocol::http::Header::CONNECTION, oatpp::web::protocol::http::Header::Value::CONNECTION_CLOSE);

        for (const auto& pair : headers.getAll()) {
            response->putHeader_Unsafe(pair.first, pair.second);
        }

        return response;
    };

};

#endif