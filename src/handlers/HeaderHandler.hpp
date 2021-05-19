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
        response->putHeaderIfNotExists("Content-Type", "application/json");
        return response;
    }
};

#endif