#ifndef handlers_HeaderHandler_hpp_included
#define handlers_HeaderHandler_hpp_included

#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"
#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"

class BaseHeader : public oatpp::web::server::interceptor::ResponseInterceptor {
public:
    BaseHeader() {}

    std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request,
        const std::shared_ptr<OutgoingResponse>& response) override
    {
        static const char* m_origin = "*";
        static const char* m_methods = "GET, POST, OPTIONS, PUT, DELETE, PATCH";
        static const char* m_headers = "User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization";
        static const char* m_max_age = "1728000";

        response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_ORIGIN, m_origin);
        response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_METHODS, m_methods);
        response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_HEADERS, m_headers);
        response->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_MAX_AGE, m_max_age);
        response->putHeaderIfNotExists("Content-Type", "application/json");
        return response;
    }
};

#endif