#ifndef controller_AuthController_hpp_included
#define controller_AuthController_hpp_included

#include "Globals.hpp"
#include "handlers/AuthorizationHandler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "database/tables/UsersTable.hpp"
#include "database/tables/OtherTable.hpp"

#include <algorithm>
#include <regex>
#include "utils/hash.hpp"
#include "utils/utils.hpp"

using hash = aru::hash;

#include OATPP_CODEGEN_BEGIN(ApiController)

class AuthController : public oatpp::web::server::api::ApiController
{
private:
	typedef AuthController __ControllerType;
public:
	AuthController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{
		setDefaultAuthorizationHandler(std::make_shared<TokenAuthorizationHandler>());
	}

	ENDPOINT("POST", "/login", loginUser,
		AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, userInfo))
	{
		if (authObject->valid)
		{
			const tables::tokens tokens_table{};
			auto db(aru::ConnectionPool::getInstance()->getConnection());
			db(sqlpp::update(tokens_table).set(
				tokens_table.last_updated = aru::time_convert::getEpochNow()
			).where(tokens_table.token == authObject->token->c_str()));

			json ok;
			ok["id"] = (*authObject->userID);
			ok["token"] = authObject->token->c_str();
			auto response = createResponse(Status::CODE_200, ok.dump().c_str());
			response->putHeader("set-cookie",
				fmt::format(
					"hat={}; Path=/; Domain={}; Max-Age={}; Secure",
					authObject->token->c_str(),
					config::frontend_site,
					14 * 24 * 60 * 60 // 14 days
				).c_str()
			);
			return response;
		}

		json body = json::parse(userInfo->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["username"].is_null() || body["password"].is_null())
				return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());

			const std::string& password = body["password"];
			std::string username = aru::utils::str_tolower(body["username"]);
			aru::utils::trim(username);
			std::replace(username.begin(), username.end(), ' ', '_');

			const tables::users users_table{};
			auto db(aru::ConnectionPool::getInstance()->getConnection());
			auto result = db(sqlpp::select(
				users_table.id, users_table.password_md5, users_table.salt
			).from(users_table).where(users_table.safe_username == username).limit(1u));

			if (result.empty())
				return createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Wrong login").c_str());

			const auto& row = result.front();
			int32_t userID = row.id;

			if (userID == 1)
				return createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "No.").c_str());

			if (hash::createSHA512(hash::createMD5(password), row.salt) == row.password_md5.value())
			{
				const tables::tokens tokens_table{};
				std::string token;

				// Delete every old token of this user
				db(sqlpp::remove().from(tokens_table).where(tokens_table.user == userID and tokens_table._private == true));

				while (true)
				{
					token = hash::createMD5(aru::utils::genRandomString(25));
					auto result = db(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.token == token).limit(1u));
					if (result.empty())
					{
						db(sqlpp::insert_into(tokens_table).set(
							tokens_table.user = userID,
							tokens_table.token = token,
							tokens_table._private = true,
							tokens_table.privileges = 0,
							tokens_table.last_updated = aru::time_convert::getEpochNow()
						));
						break;
					}

					// This token exist, create next one!
					result.pop_front();
				}


				json response;
				response["id"] = userID;
				response["token"] = token;
				auto wait = createResponse(Status::CODE_200, response.dump().c_str());
				wait->putHeader("set-cookie",
					fmt::format(
						"hat={}; Path=/; Domain={}; Max-Age={}; Secure",
						token,
						config::frontend_site,
						14 * 24 * 60 * 60 // 14 days
					).c_str()
				);
				return wait;
			}

			return createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Wrong login").c_str());
		}

		return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());
	}

	ENDPOINT("POST", "/register", registerUser, BODY_STRING(String, userInfo), REQUEST(std::shared_ptr<IncomingRequest>, request))
	{
		json body = json::parse(userInfo->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["username"].is_null() || body["password"].is_null() || body["email"].is_null())
				return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());

			std::string username = body["username"];
			aru::utils::trim(username);
			std::string safe_username = username;
			aru::utils::str_tolower(safe_username);
			std::replace(safe_username.begin(), safe_username.end(), ' ', '_');

			static std::regex user_regex("^[A-Za-z0-9 _[\\]-]{2,15}$");
			if (!std::regex_search(username, user_regex))
				return createResponse(Status::CODE_403, 
					aru::createError(521, "This nickname contains forbidden symbols. Allowed symbols: a-Z 0-9 _[]-").c_str());

			if (username.find('_') != std::string::npos && username.find(' ') != std::string::npos)
				return createResponse(Status::CODE_403,
					aru::createError(522, "Nickname should not contain spaces and underscores at the same time.").c_str());

			for (std::string& nick : config::forbidden_nicknames)
				if (nick == safe_username)
					return createResponse(Status::CODE_403, 
						aru::createError(523, "This nickname are forbidden. If you are real owner of this nickname, please contact us.").c_str());

			const tables::users users_table{};
			auto db(aru::ConnectionPool::getInstance()->getConnection());
			auto result1 = db(sqlpp::select(users_table.id).from(users_table).where(users_table.safe_username == safe_username || users_table.username == username).limit(1u));

			if (!result1.empty())
				return createResponse(Status::CODE_403, aru::createError(524, "This nickname already taken!").c_str());
			result1.pop_front();

			const std::string& email = body["email"];
			auto result2 = db(sqlpp::select(users_table.email).from(users_table).where(users_table.email == email).limit(1u));
			if (!result2.empty())
				return createResponse(Status::CODE_403, aru::createError(525, "This email already taken!").c_str());
			result2.pop_front();

			std::string salt = aru::utils::genRandomString(24);
			std::string password = hash::createSHA512(hash::createMD5(body["password"]), salt);
			db(sqlpp::insert_into(users_table).set(
				users_table.username = username,
				users_table.safe_username = safe_username,
				users_table.country = "XX",
				users_table.email = email,
				users_table.password_md5 = password,
				users_table.salt = salt,
				users_table.ip = aru::utils::getIPAddress(request),
				users_table.registration_date = aru::time_convert::getEpochNow(),
				users_table.roles = 3
			));

			auto result = db(sqlpp::select(users_table.id).from(users_table).where(users_table.email == email).limit(1u));
			int32_t user_id = result.front().id;
			result.pop_front();

			const tables::users_stats users_stats_table{};
			const tables::users_stats_relax users_stats_relax_table{};
			const tables::users_preferences users_preferences_table{};

			db(sqlpp::insert_into(users_stats_table).set(users_stats_table.id = user_id));
			db(sqlpp::insert_into(users_stats_relax_table).set(users_stats_relax_table.id = user_id));
			db(sqlpp::insert_into(users_preferences_table).set(users_preferences_table.id = user_id));

			const tables::tokens tokens_table{};
			std::string token;

			while (true)
			{
				token = hash::createMD5(aru::utils::genRandomString(25));
				auto result = db(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.token == token).limit(1u));
				if (result.empty())
				{
					db(sqlpp::insert_into(tokens_table).set(
						tokens_table.user = user_id,
						tokens_table.token = token,
						tokens_table._private = true,
						tokens_table.privileges = 0,
						tokens_table.last_updated = aru::time_convert::getEpochNow()
					));
					break;
				}

				// This token exist, create next one!
				result.pop_front();
			}

			json response;
			response["id"] = user_id;
			response["token"] = token;
			auto wait = createResponse(Status::CODE_201, response.dump().c_str());
			wait->putHeader("set-cookie",
				fmt::format(
					"hat={}; Path=/; Domain={}; Max-Age={}; Secure",
					token,
					config::frontend_site,
					14 * 24 * 60 * 60 // 14 days
				).c_str()
			);
			return wait;
		}

		return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());
	}

	ENDPOINT("GET", "/tokens/{id}", getTokens, PATH(Int32, id), AUTHORIZATION(std::shared_ptr<TokenObject>, authObject))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized").c_str());
		if (authObject->userID != id)
			return createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden").c_str());

		const tables::tokens tokens_table{};
		auto db(aru::ConnectionPool::getInstance()->getConnection());
		auto result = db(sqlpp::select(
			tokens_table.token, tokens_table.privileges
		).from(tokens_table).where(tokens_table.user == (*id) and tokens_table._private == false));

		json response = json::array();
		for (const auto& row : result)
		{
			json token;
			token["token"]      = row.token.value();
			token["privileges"] = row.privileges.value();
			response.push_back(token);
		}

		return createResponse(Status::CODE_200, response.dump().c_str());
	}

	ENDPOINT("POST", "/tokens/{id}", createToken, PATH(Int32, id), 
		AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, userInfo))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized").c_str());
		if (authObject->userID != id)
			return createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden").c_str());

		json body = json::parse(userInfo->c_str(), nullptr, false);
		int32_t privileges = 0;
		if (!body.is_discarded())
			if (body["privileges"].is_number_integer())
				privileges = body["privileges"];

		auto db(aru::ConnectionPool::getInstance()->getConnection());

		if (privileges > 0)
		{
			const tables::users users_table{};
			auto check_privileges = db(sqlpp::select(users_table.roles).from(users_table).where(users_table.id == (*id)).limit(1u));

			const auto& row = check_privileges.front();
			if (privileges > row.roles)
				privileges = row.roles;
		}

		const tables::tokens tokens_table{};
		std::string token;

		while (true)
		{
			token = hash::createMD5(aru::utils::genRandomString(25));
			auto result = db(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.token == token).limit(1u));
			if (result.empty())
			{
				db(sqlpp::insert_into(tokens_table).set(
					tokens_table.user = (*id),
					tokens_table.token = token,
					tokens_table._private = false,
					tokens_table.privileges = privileges,
					tokens_table.last_updated = aru::time_convert::getEpochNow()
				));
				break;
			}

			// This token exist, create next one!
			result.pop_front();
		}

		json response;
		response["token"] = token;
		response["privileges"] = privileges;
		return createResponse(Status::CODE_201, response.dump().c_str());
	}

	ENDPOINT("DELETE", "/tokens/{id}", deleteToken, PATH(Int32, id), 
		AUTHORIZATION(std::shared_ptr<TokenObject>, authObject))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized").c_str());
		if (authObject->userID != id)
			return createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden").c_str());

		const tables::tokens tokens_table{};
		auto db(aru::ConnectionPool::getInstance()->getConnection());
		db(sqlpp::remove_from(tokens_table).where(tokens_table.token == authObject->token->c_str()));

		json response;
		response["message"] = "Bye!";
		return createResponse(Status::CODE_200, response.dump().c_str());
	}

};

#include OATPP_CODEGEN_END(ApiController)

#endif
