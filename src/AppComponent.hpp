#ifndef AppComponent_hpp_included
#define AppComponent_hpp_included

#include "Globals.hpp"
#include "handlers/ErrorHandler.hpp"
#include "handlers/HeaderHandler.hpp"
#include "handlers/RateLimitHandler.hpp"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/component.hpp"


class AppComponent {
public:
	// Database config
	OATPP_CREATE_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config)([]
	{
		auto config = std::make_shared<sqlpp::mysql::connection_config>();
		config->host = config::database::host;
		config->port = config::database::port;
		config->user = config::database::username;
		config->password = config::database::password;
		config->database = config::database::database;
		config->debug = config::database::debug;
		return config;
	}());

	// Redis
	OATPP_CREATE_COMPONENT(std::shared_ptr<cpp_redis::client>, m_redis)([]
	{
		cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger());
		std::shared_ptr<cpp_redis::client> client = std::make_shared<cpp_redis::client>();
		client->connect(config::redis::address, config::redis::port);
		if (!config::redis::password.empty())
			client->auth(config::redis::password);

		return client;
	}());

	// Create Async Executor
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
		return std::make_shared<oatpp::async::Executor>(9, 2, 1);
	}());

	// ConnectionProvider component which listens on the port
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([]
	{
		return oatpp::network::tcp::server::ConnectionProvider::createShared({ "0.0.0.0", 8000, oatpp::network::Address::IP_4 });
	}());

	// Router component
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([]
	{
		return oatpp::web::server::HttpRouter::createShared();
	}());

	// ConnectionHandler component which uses Router component to route requests
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
		OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
		auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);

		/* Setup custom error page */
		connectionHandler->setErrorHandler(JsonErrorHandler::createShared());

		/* Add Rate Limit interceptor if allowed in config */
		if (config::limits::enable_rate_limit)
			connectionHandler->addRequestInterceptor(std::make_shared<RateLimit>());

		/* Add CORS interceptor to allow use API everywhere */
		using namespace oatpp::web::server::interceptor;
		connectionHandler->addRequestInterceptor(std::make_shared<AllowOptionsGlobal>());
		connectionHandler->addResponseInterceptor(std::make_shared<AllowCorsGlobal>("*", "GET, POST, OPTIONS, PUT, DELETE, PATCH"));

		/* Set Content-Type header */
		connectionHandler->addResponseInterceptor(std::make_shared<BaseHeader>());

		return connectionHandler;
	}());

	// ObjectMapper component to serialize/deserialize DTOs in Contoller's API
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([]
	{
		return oatpp::parser::json::mapping::ObjectMapper::createShared();
	}());

};

#endif
