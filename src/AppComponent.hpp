#ifndef AppComponent_hpp_included
#define AppComponent_hpp_included

#include "Globals.hpp"
#include "handlers/ErrorHandler.hpp"
#include "handlers/HeaderHandler.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
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
		config->port = config::database::port;
		config->user = config::database::username;
		config->password = config::database::password;
		config->database = config::database::database;
		config->debug = config::database::debug;
		return config;
	}());

	// Redis
	OATPP_CREATE_COMPONENT(std::shared_ptr<himitsu::redis>, m_redis)([]
	{
		cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger());
		himitsu::redis client(
			config::redis::address,
			config::redis::port,
			config::redis::reconnection_attempts,
			config::redis::password
		);
		client.connect();
		return std::make_shared<himitsu::redis>(client);
	}());

	// ConnectionProvider component which listens on the port
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([]
	{
		return oatpp::network::tcp::server::ConnectionProvider::createShared({ "0.0.0.0", 8000 });
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
		auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

		/* Setup custom error page */
		connectionHandler->setErrorHandler(JsonErrorHandler::createShared());
		/* Add CORS request and response interceptors */
		connectionHandler->addRequestInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>());
		connectionHandler->addResponseInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>(
			"*",
			"GET, POST, OPTIONS, PUT, DELETE, PATCH"
		));
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
