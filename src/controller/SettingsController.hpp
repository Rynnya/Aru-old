#ifndef controller_SettingsController_hpp_included
#define controller_SettingsController_hpp_included

#include <filesystem>

#include "Globals.hpp"
#include "handlers/AuthorizationHandler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/core/data/stream/FileStream.hpp"

#include "handlers/PoolHandler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class SettingsController : public oatpp::web::server::api::ApiController
{
private:
	typedef SettingsController __ControllerType;
	std::shared_ptr<TokenAuthorizationHandler> tokenAuth = std::make_shared<TokenAuthorizationHandler>();
	std::shared_ptr<SettingsController::OutgoingResponse> getSettings(const aru::Connection& db, int32_t id) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateBackground(const aru::Connection& db, int32_t id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateUserpage(const aru::Connection& db, int32_t id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateStatus(const aru::Connection& db, int32_t id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updatePref(const aru::Connection& db, int32_t id, json body) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateScoreboard(const aru::Connection& db, int32_t id, json body) const;
	SettingsController(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<SettingsController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<SettingsController>(new SettingsController(objectMapper));
	}

	ENDPOINT_ASYNC("GET", "/users/{id}/settings", getUserSettings)
	{
		ENDPOINT_ASYNC_INIT(getUserSettings);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&getUserSettings::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			return _return(controller->getSettings(db, user_id));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/background", changeBackground)
	{
		ENDPOINT_ASYNC_INIT(changeBackground);

		int32_t user_id = -1;
		oatpp::String body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return request->readBodyToStringAsync().callbackTo(&changeBackground::onBody);
		}

		Action onBody(const oatpp::String& request_body)
		{
			body = request_body;
			return PoolHandler::startForResult().callbackTo(&changeBackground::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			return _return(controller->updateBackground(db, user_id, body->c_str()));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/userpage", changeUserpage)
	{
		ENDPOINT_ASYNC_INIT(changeUserpage);

		int32_t user_id = -1;
		oatpp::String body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return request->readBodyToStringAsync().callbackTo(&changeUserpage::onBody);
		}

		Action onBody(const oatpp::String& request_body)
		{
			body = request_body;
			return PoolHandler::startForResult().callbackTo(&changeUserpage::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			return _return(controller->updateUserpage(db, user_id, body->c_str()));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/status", changeStatus)
	{
		ENDPOINT_ASYNC_INIT(changeStatus);

		int32_t user_id = -1;
		oatpp::String body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return request->readBodyToStringAsync().callbackTo(&changeStatus::onBody);
		}

		Action onBody(const oatpp::String& request_body)
		{
			body = request_body;
			return PoolHandler::startForResult().callbackTo(&changeStatus::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			return _return(controller->updateStatus(db, user_id, body->c_str()));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/avatar", changeAvatar)
	{
		ENDPOINT_ASYNC_INIT(changeAvatar);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&changeAvatar::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			if (config::avatar_folder == "/path/to/folder/{}.png")
			{
				fmt::print("Warning! Avatars currently disabled, please change example directory.\n");
				return _return(controller->createResponse(Status::CODE_422, aru::createError(Status::CODE_422, "Sorry, but avatars currently not available to change!")));
			}

			auto size = request->getHeader("Content-Length");
			if (!size)
			{
				fmt::print("Warning: User {} trying to send image without Content-Length header.", user_id);
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Image cannot be sended without Content-Length header")));
			}

			if (aru::convert::safe_int(size, 1048577) > 1048576 /* 1024 * 1024 */)
			{
				fmt::print("Warning: User {} trying to send really huge image (or maybe not image). Size: {}\n", user_id, size->c_str());
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Image files cannot be more than 1 MB")));
			}

			std::shared_ptr<oatpp::data::stream::FileOutputStream> fileOutputStream = std::make_shared<oatpp::data::stream::FileOutputStream>(fmt::format(config::avatar_folder, user_id).c_str());
			return request->transferBodyToStreamAsync(fileOutputStream).next(yieldTo(&changeAvatar::finish));
		}

		Action finish()
		{
			OATPP_COMPONENT(std::shared_ptr<cpp_redis::client>, redis);
			redis->publish("aru.change_avatar", std::to_string(user_id));
			redis->commit();

			return _return(controller->createResponse(Status::CODE_200, "OK"));
		}
	};

	ENDPOINT_ASYNC("DELETE", "/users/{id}/settings/avatar", deleteAvatar)
	{
		ENDPOINT_ASYNC_INIT(deleteAvatar);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&deleteAvatar::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			if (config::avatar_folder == "/path/to/folder/{}.png")
			{
				fmt::print("Warning! Avatars currently disabled, please change example directory.\n");
				return _return(controller->createResponse(Status::CODE_422, aru::createError(Status::CODE_422, "Sorry, but avatars currently not available to change!")));
			}

			std::filesystem::path file = fmt::format(config::avatar_folder, user_id);
			if (std::filesystem::exists(file))
			{
				std::filesystem::remove(file);

				OATPP_COMPONENT(std::shared_ptr<cpp_redis::client>, redis);
				redis->publish("aru::change_avatar", std::to_string(user_id));
				redis->commit();
				return _return(controller->createResponse(Status::CODE_200, "OK"));
			}

			return _return(controller->createResponse(Status::CODE_404, ""));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/pref", changeFavouriteMode)
	{
		ENDPOINT_ASYNC_INIT(changeFavouriteMode);

		int32_t user_id = -1;
		json body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return request->readBodyToStringAsync().callbackTo(&changeFavouriteMode::onBody);
		}

		Action onBody(const oatpp::String& request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			if (body.is_discarded())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&changeFavouriteMode::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			return _return(controller->updatePref(db, user_id, body));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/scoreboard", changeScoreboard)
	{
		ENDPOINT_ASYNC_INIT(changeScoreboard);

		int32_t user_id = -1;
		json body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return request->readBodyToStringAsync().callbackTo(&changeScoreboard::onBody);
		}

		Action onBody(const oatpp::String& request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			if (body.is_discarded())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&changeScoreboard::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			return _return(controller->updateScoreboard(db, user_id, body));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
