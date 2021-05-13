#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "Globals.h"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/component.hpp"

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:
	/**
	*  Create Database config
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<mysql::connection_config>, config)([]
	{
		auto config = std::make_shared<mysql::connection_config>();
		config->port = config::db_port;
		config->user = config::db_user;
		config->password = config::db_password;
		config->database = config::db_database;
		config->debug = config::db_debug;
		return config;
	}());
	/**
	*  Create Redis instance
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<himitsu::redis>, m_redis)([]
	{
		cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger());
		himitsu::redis client(
			config::redis_address,
			config::redis_port,
			config::redis_reconnection_attempts,
			config::redis_password
		);
		client.connect();
		return std::make_shared<himitsu::redis>(client);
	}());

	/**
	 *  Create ConnectionProvider component which listens on the port
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([]
	{
		return oatpp::network::tcp::server::ConnectionProvider::createShared({ "0.0.0.0", 8000 });
	}());

	/**
	 *  Create Router component
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([]
	{
		return oatpp::web::server::HttpRouter::createShared();
	}());

	/**
	 *  Create ConnectionHandler component which uses Router component to route requests
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
		return oatpp::web::server::HttpConnectionHandler::createShared(router);
	}());

	/**
	 *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([]
	{
		return oatpp::parser::json::mapping::ObjectMapper::createShared();
	}());

};

#endif /* AppComponent_hpp */
