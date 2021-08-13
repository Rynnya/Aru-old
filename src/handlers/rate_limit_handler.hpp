#ifndef handlers_rate_limit_handler_hpp_included
#define handlers_rate_limit_handler_hpp_included

#include "fmt/core.h"
#include "../controller/auth_controller.hpp"

#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"
#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"

class rate_limit : public oatpp::web::server::interceptor::RequestInterceptor
{
private:
    std::unordered_map<std::string, std::atomic_uint32_t> _limiter;
public:

    rate_limit()
    {
        std::thread(&rate_limit::cleanup, this).detach();
        fmt::print(
            "Rate Limiter: Enabled with {} requests/{} seconds ({} minutes).\n",
            config::limits::limit, config::limits::timer, std::floor(config::limits::timer / 60));
    }

    static std::shared_ptr<rate_limit> create_shared()
    {
        return std::make_shared<rate_limit>();
    }

    std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override
    {
        const std::string& ip = aru::utils::get_ip_address(request);
        if (_limiter[ip]++ > config::limits::limit)
        {
            using namespace oatpp::web::protocol::http;
            static oatpp::String error = aru::create_error(
                Status::CODE_429,
                fmt::format("Rate limit occurred ({} requests/{} seconds)", config::limits::limit, config::limits::timer)
            );
            return outgoing::ResponseFactory::createResponse(Status::CODE_429, error);
        }

        return nullptr;
    }

    void cleanup()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(config::limits::timer));
            _limiter.clear();
        }
    }
};

#endif
