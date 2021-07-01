#ifndef handlers_RateLimitHandler_hpp_included
#define handlers_RateLimitHandler_hpp_included

#include "fmt/core.h"
#include "../controller/AuthController.hpp"

#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"
#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"

class RateLimit : public oatpp::web::server::interceptor::RequestInterceptor {
private:
    std::unordered_map<std::string, uint32_t> _limiter;
    std::mutex _mtx;
public:
    RateLimit()
    {
        std::thread(&RateLimit::cleanup, this).detach();
        fmt::print(
            "Rate Limiter: Enabled with {} requests/{} seconds ({} minutes).\n",
            config::limits::limit, config::limits::timer, std::floor(config::limits::timer / 60));
    }

    std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override
    {
        std::string ip = aru::utils::getIPAddress(request);
        if (_limiter[ip] > config::limits::limit)
        {
            using namespace oatpp::web::protocol::http;
            return outgoing::ResponseFactory::createResponse(Status::CODE_429, 
                aru::createError(Status::CODE_429, fmt::format("Rate limit occurred ({} requests/{} seconds)", config::limits::limit, config::limits::timer)).c_str());
        }
        std::unique_lock mutex(_mtx);
        _limiter[ip]++;
        mutex.unlock();
        return nullptr;
    }

    void cleanup()
    {
        std::unique_lock mutex(_mtx);
        mutex.unlock();
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(config::limits::timer));
            mutex.lock();
            _limiter.clear();
            mutex.unlock();
        }
    }
};

#endif
