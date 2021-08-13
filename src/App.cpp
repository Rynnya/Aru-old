#include "components.hpp"

#include "controller/auth_controller.hpp"
#include "controller/beatmap_controller.hpp"
#include "controller/base_controller.hpp"
#include "controller/settings_controller.hpp"
#include "controller/users_controller.hpp"

#include "oatpp/network/Server.hpp"
#include "handlers/native_handler.hpp"
#include "database/sql.hpp"

#include <iostream>

std::thread server_thread;
std::shared_ptr<oatpp::network::Server> server;

void on_exit()
{
	fmt::print("Server: Shutting down...\n");

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connection_handler);
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connection_provider);

	/* First, stop the ServerConnectionProvider so we don't accept any new connections - actually do nothing */
	connection_provider->stop();

	/* Now, check if server is still running and stop it if needed */
	if (server->getStatus() == oatpp::network::Server::STATUS_RUNNING)
	{
		server->stop();
	}

	/* Finally, stop the ConnectionHandler and wait until all running connections are closed - actually do nothing */
	connection_handler->stop();

	/* Wait until server done his job */
	if (server_thread.joinable())
	{
		server_thread.join();
	}

	/* Cleanup pool - We can safely use this because every connection now closed and database are not used (except detached thread witch cannot throw exceptions for us) */
	aru::connection_pool::get_instance()->~connection_pool();

	/* oatpp::base::Environment::destroy(); do same thing but crash program because of existing AppComponent */
	#if defined(WIN32) || defined(_WIN32)
		WSACleanup();
	#endif

	/* We can safely stop the program because everything that exist right now can be removed by system */
	fmt::print("Server: Bye!\n");
}

void run()
{
	/* Create exit handler */
	std::atexit(on_exit);
	native::install();

	/* Parse config file */
	config::parse();

	/* Register Components in scope of run() method */
	components _components;

	/* Get router component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	/* Add all endpoints to router */
	auto auth_controller = auth_controller::create_shared();
	auto beatmap_controller = beatmap_controller::create_shared();
	auto settings_controller = settings_controller::create_shared();
	auto base_controller = base_controller::create_shared();
	auto users_controller = users_controller::create_shared();

	auth_controller->addEndpointsToRouter(router);
	beatmap_controller->addEndpointsToRouter(router);
	settings_controller->addEndpointsToRouter(router);
	base_controller->addEndpointsToRouter(router);
	users_controller->addEndpointsToRouter(router);

	/* Get connection handler component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	/* Get connection provider component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	/* Create server which takes provided TCP connections and passes them to HTTP connection handler */
	server = oatpp::network::Server::createShared(connectionProvider, connectionHandler);

	/* Initialize database pool */
	aru::connection_pool pool(config::database::connection_amount);
	std::thread([&]
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::hours(6));
			pool.wakeUpPool();
		}
	}).detach();

	/* Priny info about server port */
	fmt::print("Server: Running on port {}\n", connectionProvider->getProperty("port").getData());
	fmt::print("Server: Press CTRL + C to exit.\n");

	/* Run server */
	server_thread = std::move(std::thread([] { server->run(); }));
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::hours(240));
	}
}

int main(int argc, const char* argv[])
{
	/* WinSock2.h don't make sense here, because OATPP uses the same way to communicate */
	/* Check https://github.com/oatpp/oatpp/blob/master/src/oatpp/core/base/Environment.cpp#L179 */

	oatpp::base::Environment::init();
	run();

	return 0;
}
