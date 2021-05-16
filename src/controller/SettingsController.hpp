#ifndef SettingsController_hpp
#define SettingsController_hpp

#include "Globals.hpp"

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
	{}
private:
	std::shared_ptr<SettingsController::OutgoingResponse> updateBackground(Int32 id,std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateUserpage(Int32 id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateStatus(Int32 id, std::string request) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updatePlayStyle(Int32 id, int body) const;
	std::shared_ptr<SettingsController::OutgoingResponse> updateScoreboard(Int32 id, int pref, int auto_classic, int auto_relax) const;
	bool checkToken(int id, String token);
public:

	ENDPOINT("POST", "/users/{id}/settings/background", changeBackground, 
		PATH(Int32, id), REQUEST(std::shared_ptr<IncomingRequest>, request), BODY_STRING(String, userInfo))
	{
		OATPP_ASSERT_HTTP(checkToken((*id), request->getHeader("Authorization")), Status::CODE_401, "Unauthorized");
		return updateBackground(id, userInfo->c_str());
	};

	ENDPOINT("POST", "/users/{id}/settings/userpage", changeUserpage, 
		PATH(Int32, id), REQUEST(std::shared_ptr<IncomingRequest>, request), BODY_STRING(String, userInfo))
	{
		OATPP_ASSERT_HTTP(checkToken((*id), request->getHeader("Authorization")), Status::CODE_401, "Unauthorized");
		return updateUserpage(id, userInfo->c_str());
	};

	ENDPOINT("POST", "/users/{id}/settings/status", changeStatus,
		PATH(Int32, id), REQUEST(std::shared_ptr<IncomingRequest>, request), BODY_STRING(String, userInfo))
	{
		OATPP_ASSERT_HTTP(checkToken((*id), request->getHeader("Authorization")), Status::CODE_401, "Unauthorized");
		return updateStatus(id, userInfo->c_str());
	};

	ENDPOINT("POST", "/users/{id}/settings/avatar", changeAvatar, PATH(Int32, id), REQUEST(std::shared_ptr<IncomingRequest>, request))
	{
		OATPP_ASSERT_HTTP(checkToken((*id), request->getHeader("Authorization")), Status::CODE_401, "Unauthorized");
		oatpp::data::stream::FileOutputStream fileOutputStream(fmt::format(config::avatar_folder, (*id)).c_str());
		request->transferBodyToStream(&fileOutputStream);
		return createResponse(Status::CODE_200, "OK");
	};

	ENDPOINT("POST", "/users/{id}/settings/playstyle", changePlayStyle, 
		PATH(Int32, id), REQUEST(std::shared_ptr<IncomingRequest>, request), BODY_STRING(String, playstyle))
	{
		OATPP_ASSERT_HTTP(checkToken((*id), request->getHeader("Authorization")), Status::CODE_401, "Unauthorized");

		json body = json::parse(playstyle.get()->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["playstyle"].is_null() || !body["playstyle"].is_number_integer())
				return createResponse(Status::CODE_204, "No data provided");

			return updatePlayStyle(id, body["playstyle"]);
		}

		return createResponse(Status::CODE_400, "Bad request");
	};

	ENDPOINT("POST", "/users/{id}/settings/scoreboard", changeScoreboard, 
		PATH(Int32, id), REQUEST(std::shared_ptr<IncomingRequest>, request), BODY_STRING(String, scoreboard))
	{
		OATPP_ASSERT_HTTP(checkToken((*id), request->getHeader("Authorization")), Status::CODE_401, "Unauthorized");

		json body = json::parse(scoreboard.get()->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["preferences"].is_null() || !body["preferences"].is_number_integer())
				return createResponse(Status::CODE_204, "No data provided");

			if (body["auto_classic"].is_null() || !body["auto_classic"].is_number_integer())
				return createResponse(Status::CODE_204, "No data provided");

			if (body["auto_relax"].is_null() || !body["auto_relax"].is_number_integer())
				return createResponse(Status::CODE_204, "No data provided");

			return updateScoreboard(id, body["preferences"], body["auto_classic"], body["auto_relax"]);
		}

		return createResponse(Status::CODE_400, "Bad request");
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif
