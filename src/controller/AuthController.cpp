#include "AuthController.hpp"

std::string AuthController::getIPAddress(std::shared_ptr<IncomingRequest> request)
{
	oatpp::String address = request->getHeader("CF-Connecting-IP");
	if (address)
		return address->c_str();

	address = request->getHeader("X-Real-IP");
	if (address)
		return address->c_str();

	return "0.0.0.0";
}