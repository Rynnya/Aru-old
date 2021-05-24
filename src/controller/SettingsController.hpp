#ifndef controller_SettingsController_hpp_included
#define controller_SettingsController_hpp_included

#include "Globals.hpp"
#include "handlers/AuthorizationHandler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/core/data/stream/FileStream.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class SettingsController : public oatpp::web::server::api::ApiController {
private:
	typedef SettingsController __ControllerType;
public:
	SettingsController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{
		setDefaultAuthorizationHandler(std::make_shared<TokenAuthorizationHandler>());
	}
private:
	std::shared_ptr<SettingsController::OutgoingResponse> getSettings(Int32 id) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateBackground(Int32 id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateUserpage(Int32 id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateStatus(Int32 id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updatePref(Int32 id, int fav_mode, bool fav_relax, int playstyle) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateScoreboard(Int32 id, int pref, int auto_classic, int auto_relax) const;
public:

	ENDPOINT("GET", "/users/{id}/settings", getUserSettings,
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		return getSettings(id);
	};

	ENDPOINT("PUT", "/users/{id}/settings/background", changeBackground, 
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, userInfo))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		return updateBackground(id, userInfo->c_str());
	};

	ENDPOINT("PUT", "/users/{id}/settings/userpage", changeUserpage, 
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, userInfo))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		return updateUserpage(id, userInfo->c_str());
	};

	ENDPOINT("PUT", "/users/{id}/settings/status", changeStatus,
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, userInfo))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		return updateStatus(id, userInfo->c_str());
	};

	ENDPOINT("PUT", "/users/{id}/settings/avatar", changeAvatar,
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), REQUEST(std::shared_ptr<IncomingRequest>, request))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		if (config::avatar_folder == "/path/to/folder/{}.png")
		{
			fmt::print("Warning! Avatars currently disabled, please change example directory.\n");
			return createResponse(Status::CODE_422, himitsu::createError(Status::CODE_422, "Sorry, but avatars currently not available to change!").c_str());
		}

		auto size = request->getHeader("Content-Length");
		if (!size)
		{
			fmt::print("Warning: User {} trying to send image without Content-Length header.", (*id));
			return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "Image cannot be sended without Content-Length header").c_str());
		}

		if (std::stoi(size->c_str()) > 1024 * 1024)
		{
			fmt::print("Warning: User {} trying to send really huge image (or maybe not image). Size: {}\n", (*id), size->c_str());
			return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "Image files cannot be more than 1 MB").c_str());
		}

		oatpp::data::stream::FileOutputStream fileOutputStream(fmt::format(config::avatar_folder, (*id)).c_str());
		request->transferBodyToStream(&fileOutputStream);
		return createResponse(Status::CODE_200, "OK");
	};

	ENDPOINT("PUT", "/users/{id}/settings/pref", changeFavouriteMode,
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, fav_modes))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		json body = json::parse(fav_modes->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["favourite_mode"].is_null() || !body["favourite_mode"].is_number_integer())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided: favourite_mode").c_str());
			if (body["favourite_relax"].is_null() || !body["favourite_relax"].is_number_integer())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided: favourite_relax").c_str());
			if (body["playstyle"].is_null() || !body["playstyle"].is_number_integer())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided: playstyle").c_str());

			return updatePref(id, body["favourite_mode"], himitsu::utils::intToBoolean(body["favourite_relax"]), body["playstyle"]);
		}

		return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "Bad request").c_str());
	}

	ENDPOINT("PUT", "/users/{id}/settings/scoreboard", changeScoreboard, 
		PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, scoreboard))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Unauthorized").c_str());
		if (!(authObject->userID == id))
			return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "Forbidden").c_str());

		json body = json::parse(scoreboard->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["preferences"].is_null() || !body["preferences"].is_number_integer())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided: preferences").c_str());

			if (body["auto_classic"].is_null() || !body["auto_classic"].is_number_integer())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided: auto_classic").c_str());

			if (body["auto_relax"].is_null() || !body["auto_relax"].is_number_integer())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided: auto_relax").c_str());

			return updateScoreboard(id, body["preferences"], body["auto_classic"], body["auto_relax"]);
		}

		return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "Bad request").c_str());
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif
