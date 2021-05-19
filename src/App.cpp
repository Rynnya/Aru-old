#include "./AppComponent.hpp"

#include "./controller/AuthController.hpp"
#include "./controller/BeatmapController.hpp"
#include "./controller/MainController.hpp"
#include "./controller/SettingsController.hpp"
#include "./controller/UsersController.hpp"

#include "oatpp/network/Server.hpp"

#include <iostream>

void run() {

	/* Register Components in scope of run() method */
	AppComponent components;

	/* Get router component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	/* Add all endpoints to router */
	auto authController = std::make_shared<AuthController>();
	auto beatmapController = std::make_shared<BeatmapController>();
	auto settingsController = std::make_shared<SettingsController>();
	auto mainController = std::make_shared<MainController>();
	auto userController = std::make_shared<UsersController>();

	authController->addEndpointsToRouter(router);
	beatmapController->addEndpointsToRouter(router);
	settingsController->addEndpointsToRouter(router);
	mainController->addEndpointsToRouter(router);
	userController->addEndpointsToRouter(router);

	/* Initialize database pool */
	himitsu::ConnectionPool pool(config::db_connection_amount);

	/* Get connection handler component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	/* Get connection provider component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	/* Create server which takes provided TCP connections and passes them to HTTP connection handler */
	oatpp::network::Server server(connectionProvider, connectionHandler);

	/* Priny info about server port */
	OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

	/* Initialize pseudorandom */
	srand(time(0));

	/* Run server */
	server.run();

}

int main(int argc, const char * argv[]) {

	/* WinSock2.h don't make sense here, because OATPP use the same way to communicate */
	/* Check https://github.com/oatpp/oatpp/blob/master/src/oatpp/core/base/Environment.cpp#L179 */

	oatpp::base::Environment::init();

	run();

	/* Print how much objects were created during app running, and what have left-probably leaked */
	/* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
	std::cout << "\nEnvironment:\n";
	std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
	std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

	oatpp::base::Environment::destroy();

	return 0;
}
