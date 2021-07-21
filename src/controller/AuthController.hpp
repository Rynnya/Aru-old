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

#include "handlers/PoolHandler.hpp"

using hash = aru::hash;

#include OATPP_CODEGEN_BEGIN(ApiController)

class AuthController : public oatpp::web::server::api::ApiController
{
private:
	typedef AuthController __ControllerType;
	std::shared_ptr<TokenAuthorizationHandler> tokenAuth = std::make_shared<TokenAuthorizationHandler>();
	AuthController(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<AuthController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<AuthController>(new AuthController(objectMapper));
	}

	ENDPOINT_ASYNC("POST", "/login", loginUser)
	{
		ENDPOINT_ASYNC_INIT(loginUser);

		json body = nullptr;

		Action act() override
		{
			return request->readBodyToStringAsync().callbackTo(&loginUser::onBody);
		}

		Action onBody(const oatpp::String & request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			return PoolHandler::startForResult().callbackTo(&loginUser::onDatabase);
		}

		Action onDatabase(const aru::Connection & db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (authObject->valid)
			{
				const tables::tokens tokens_table{};
				OATPP_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config);
				sqlpp::mysql::connection db(config);

				db(sqlpp::update(tokens_table).set(
					tokens_table.last_updated = aru::time_convert::getEpochNow()
				).where(tokens_table.token == authObject->token));

				json ok;
				ok["id"] = authObject->userID;
				ok["token"] = authObject->token;
				auto response = controller->createResponse(Status::CODE_200, ok.dump().c_str());
				response->putHeader("set-cookie",
					fmt::format(
						"hat={}; Path=/; Domain={}; Max-Age={}; Secure",
						authObject->token,
						config::frontend_site,
						1209600 /* 2 weeks */
					).c_str()
				);
				return _return(response);
			}

			if (body.is_discarded())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			if (!body["username"].is_string() || !body["password"].is_string())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			std::string password = body["password"];
			std::string username = aru::utils::str_tolower(body["username"]);
			aru::utils::trim(username);
			std::replace(username.begin(), username.end(), ' ', '_');

			const tables::users users_table{};

			auto result = db(sqlpp::select(
				users_table.id, users_table.password_md5, users_table.salt
			).from(users_table).where(users_table.safe_username == username).limit(1u));

			if (result.empty())
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Wrong login")));

			const auto& row = result.front();
			int32_t userID = row.id;

			if (userID == 1)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "No.")));

			if (hash::createSHA512(hash::createMD5(password), row.salt) != row.password_md5.value())
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Wrong login")));

			const tables::tokens tokens_table{};
			std::string token;

			// Delete every old token of this user
			db(sqlpp::remove_from(tokens_table).where(tokens_table.user == userID and tokens_table._private == true));

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

				// This token exist, create next one! (We check this because we can have same token as another user, oops...)
				result.pop_front();
			}

			json response;
			response["id"] = userID;
			response["token"] = token;
			auto wait = controller->createResponse(Status::CODE_200, response.dump().c_str());
			wait->putHeader("set-cookie",
				fmt::format(
					"hat={}; Path=/; Domain={}; Max-Age={}; Secure",
					token,
					config::frontend_site,
					1209600 /* 2 weeks */
				).c_str()
			);
			return _return(wait);
		}
	};

	ENDPOINT_ASYNC("POST", "/register", registerUser)
	{
		ENDPOINT_ASYNC_INIT(registerUser);

		json body = nullptr;

		Action act() override
		{
			return request->readBodyToStringAsync().callbackTo(&registerUser::onBody);
		}

		Action onBody(const oatpp::String & request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			return PoolHandler::startForResult().callbackTo(&registerUser::onDatabase);
		}

		Action onDatabase(const aru::Connection & db)
		{
			if (body.is_discarded())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			if (!body["username"].is_string() || !body["password"].is_string() || !body["email"].is_string())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			std::string username = body["username"];
			aru::utils::trim(username);
			std::string safe_username = username;
			aru::utils::str_tolower(safe_username);
			std::replace(safe_username.begin(), safe_username.end(), ' ', '_');

			static std::regex user_regex("^[A-Za-z0-9 _[\\]-]{2,15}$");
			if (!std::regex_search(username, user_regex))
				return _return(controller->createResponse(Status::CODE_403,
					aru::createError(521, "This nickname contains forbidden symbols. Allowed symbols: a-Z 0-9 _[]-")));

			if (username.find('_') != std::string::npos && username.find(' ') != std::string::npos)
				return _return(controller->createResponse(Status::CODE_403,
					aru::createError(522, "Nickname should not contain spaces and underscores at the same time.")));

			if (std::any_of(
				config::forbidden_nicknames.begin(),
				config::forbidden_nicknames.end(),
				[&](std::string& nickname) { return nickname == safe_username; }
			))
				return _return(controller->createResponse(Status::CODE_403,
					aru::createError(523, "This nickname is forbidden. If you are real owner of this nickname, please contact us.")));

			std::string email = body["email"];
			std::string salt = aru::utils::genRandomString(24);
			std::string password = hash::createSHA512(hash::createMD5(body["password"]), salt);

			const tables::users users_table{};
			auto result1 = db(sqlpp::select(users_table.id).from(users_table).where(users_table.safe_username == safe_username || users_table.username == username).limit(1u));

			if (!result1.empty())
				return _return(controller->createResponse(Status::CODE_403, aru::createError(524, "This nickname already taken!")));
			result1.pop_front();

			auto result2 = db(sqlpp::select(users_table.email).from(users_table).where(users_table.email == email).limit(1u));

			if (!result2.empty())
				return _return(controller->createResponse(Status::CODE_403, aru::createError(525, "This email already taken!")));
			result2.pop_front();


			int32_t user_id = db(sqlpp::insert_into(users_table).set(
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
			auto wait = controller->createResponse(Status::CODE_201, response.dump().c_str());
			wait->putHeader("set-cookie",
				fmt::format(
					"hat={}; Path=/; Domain={}; Max-Age={}; Secure",
					token,
					config::frontend_site,
					1209600 /* 2 weeks */
				).c_str()
			);
			return _return(wait);
		}
	};

	ENDPOINT_ASYNC("GET", "/tokens/{id}", getTokens)
	{
		ENDPOINT_ASYNC_INIT(getTokens);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&getTokens::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			const tables::tokens tokens_table{};

			auto result = db(sqlpp::select(
				tokens_table.token, tokens_table.privileges
			).from(tokens_table).where(tokens_table.user == user_id and tokens_table._private == false));

			json response = json::array();
			for (const auto& row : result)
			{
				json token;
				token["token"] = row.token.value();
				token["privileges"] = row.privileges.value();
				response.push_back(token);
			}

			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("POST", "/tokens/{id}", createToken)
	{
		ENDPOINT_ASYNC_INIT(createToken);

		int32_t user_id = -1;
		json body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return request->readBodyToStringAsync().callbackTo(&createToken::onBody);
		}

		Action onBody(const oatpp::String & request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			return PoolHandler::startForResult().callbackTo(&createToken::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			int32_t privileges = 0;
			if (!body.is_discarded() && body["privileges"].is_number_integer())
				privileges = body["privileges"];

			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));
			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			if (privileges > 0)
			{
				const tables::users users_table{};
				auto check_privileges = db(sqlpp::select(users_table.roles).from(users_table).where(users_table.id == user_id).limit(1u));

				const auto& row = check_privileges.front();
				if (privileges > row.roles)
					privileges = row.roles;
			}

			const tables::tokens tokens_table{};
			std::string token;

			while (true)
			{
				token = hash::createMD5(aru::utils::genRandomString(25));
				auto& result = db(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.token == token).limit(1u));
				if (result.empty())
				{
					db(sqlpp::insert_into(tokens_table).set(
						tokens_table.user = user_id,
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
			return _return(controller->createResponse(Status::CODE_201, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("DELETE", "/tokens/{id}", deleteToken)
	{
		ENDPOINT_ASYNC_INIT(deleteToken);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return PoolHandler::startForResult().callbackTo(&deleteToken::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			std::shared_ptr<TokenObject> authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));
			if (authObject->userID != user_id)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			const tables::tokens tokens_table{};
			db(sqlpp::remove_from(tokens_table).where(tokens_table.token == authObject->token));

			json response;
			response["message"] = "Bye!";
			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
