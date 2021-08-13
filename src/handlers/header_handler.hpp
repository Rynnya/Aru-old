#ifndef handlers_header_handler_hpp_included
#define handlers_header_handler_hpp_included

#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"
#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"

class response_headers : public oatpp::web::server::interceptor::ResponseInterceptor
{
public:
    response_headers() {}

    static std::shared_ptr<response_headers> create_shared()
    {
        return std::make_shared<response_headers>();
    }

    std::shared_ptr<OutgoingResponse> intercept(
        const std::shared_ptr<IncomingRequest>& request,
        const std::shared_ptr<OutgoingResponse>& response
    ) override
    {
        static const char* m_origin = "*";
        static const char* m_methods = "GET, POST, OPTIONS, PUT, DELETE, PATCH";
        static const char* m_headers = "User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization";
        static const char* m_max_age = "1728000";

        std::ignore = response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_ORIGIN, m_origin);
        std::ignore = response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_METHODS, m_methods);
        std::ignore = response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_HEADERS, m_headers);
        std::ignore = response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_MAX_AGE, m_max_age);
        std::ignore = response->putHeaderIfNotExists("Content-Type", "application/json");
        return response;
    }
};

class request_headers : public oatpp::web::server::interceptor::RequestInterceptor
{
public:
    request_headers() {}

    static std::shared_ptr<request_headers> create_shared()
    {
        return std::make_shared<request_headers>();
    }

    std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override
    {
        const auto& line = request->getStartingLine();

        if (line.method == "OPTIONS")
        {
            return OutgoingResponse::createShared(oatpp::web::protocol::http::Status::CODE_204, nullptr);
        }

        return nullptr;
    }
};

#endif