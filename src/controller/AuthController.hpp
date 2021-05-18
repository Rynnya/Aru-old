#ifndef AuthController_hpp
#define AuthController_hpp

#include "Globals.hpp"
#include "handlers/AuthorizationHandler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "database/tables/UsersTable.hpp"
#include "database/tables/OtherTable.hpp"

#include <algorithm>
#include <regex>
#include "utils/utils.hpp"
#include "utils/bcrypt.hpp"
#include "utils/md5.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class AuthController : public oatpp::web::server::api::ApiController {
private:
	typedef AuthController __ControllerType;
public:
	AuthController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{
		setDefaultAuthorizationHandler(std::make_shared<TokenAuthorizationHandler>());
	}
public:

	ENDPOINT("POST", "/login", loginUser,
		AUTHORIZATION(std::shared_ptr<TokenObject>, authObject), BODY_STRING(String, userInfo))
	{
		if (authObject->valid)
		{
			tokens t{};
			std::shared_ptr<himitsu::Connection> db = himitsu::ConnectionPool::getInstance()->getConnection();
			(**db)(sqlpp::update(t).set(
				t.last_updated = himitsu::time_convert::getEpochNow()
			).where(t.token == authObject->token->c_str()));

			json ok;
			ok["id"] = (*authObject->userID);
			auto response = createResponse(Status::CODE_200, ok.dump().c_str());
			response->putHeader("set-cookie",
				fmt::format(
					"hat={}; Path=/; Domain=himitsu.ml; Max-Age={}; Secure",
					authObject->token->c_str(),
					14 * 24 * 60 * 60 // 14 days
				).c_str()
			);
			return response;
		}

		json body = json::parse(userInfo->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["login"].is_null() || body["password"].is_null())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "Bad request").c_str());

			std::string password = body["password"];
			std::string username = himitsu::utils::trim(himitsu::utils::str_tolower(body["login"]));
			std::replace(username.begin(), username.end(), ' ', '_');

			if (username == "nebula")
				return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "No.").c_str());

			users user_t{};
			std::shared_ptr<himitsu::Connection> db = himitsu::ConnectionPool::getInstance()->getConnection();
			auto result = (**db)(sqlpp::select(user_t.id, user_t.password_md5).from(user_t).where(user_t.username_safe == username).limit(1u));

			if (result.empty())
				return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Wrong login").c_str());

			const auto& row = result.front();
			int userID = row.id;
			result.pop_front();

			if (bcrypt::verify(md5::createMD5(password), row.password_md5.value()))
			{
				tokens t{};
				std::string token = md5::createMD5(himitsu::utils::genRandomString(25));
				(**db)(sqlpp::insert_into(t).set(
					t.user = userID,
					t.token = token,
					t.t_private = true,
					t.privileges = 0,
					t.last_updated = himitsu::time_convert::getEpochNow()
				));

				json response;
				response["id"] = userID;
				auto wait = createResponse(Status::CODE_200, response.dump().c_str());
				wait->putHeader("set-cookie",
					fmt::format(
						"hat={}; Path=/; Domain=himitsu.ml; Max-Age={}; Secure",
						token,
						14 * 24 * 60 * 60 // 14 days
					).c_str()
				);
				return wait;
			}

			return createResponse(Status::CODE_401, himitsu::createError(Status::CODE_401, "Wrong login").c_str());
		}

		return createResponse(Status::CODE_400,
			himitsu::createError(Status::CODE_400, "Bad request").c_str()
		);
	}

	ENDPOINT("POST", "/register", registerUser, BODY_STRING(String, userInfo))
	{
		json body = json::parse(userInfo->c_str(), nullptr, false);
		if (!body.is_discarded())
		{
			if (body["login"].is_null() || body["password"].is_null() || body["email"].is_null())
				return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "Bad request").c_str());

			std::string username = body["login"];
			std::string username_safe = himitsu::utils::trim(himitsu::utils::str_tolower(username));
			std::replace(username_safe.begin(), username_safe.end(), ' ', '_');

			std::regex user_regex("^[A-Za-z0-9 _[\\]-]{2,15}$");
			if (!std::regex_search(username, user_regex))
				return createResponse(Status::CODE_403, 
					himitsu::createError(Status::CODE_403, 
						"This nickname contains forbidden symbols. Allowed symbols: a-Z 0-9 _[]-").c_str()
				);

			if (username.find('_') != std::string::npos && username.find(' ') != std::string::npos)
				return createResponse(Status::CODE_403,
					himitsu::createError(Status::CODE_403,
						"Nickname should not contain spaces and underscores at the same time.").c_str()
				);

			for (auto nick : config::forbidden_nicknames)
				if (nick == username_safe)
					return createResponse(Status::CODE_403, 
						himitsu::createError(Status::CODE_403, 
							"This nickname are forbidden. If you are real owner of this nickname, please contact us.").c_str()
					);

			users t_table{};
			std::shared_ptr<himitsu::Connection> db = himitsu::ConnectionPool::getInstance()->getConnection();
			auto result1 = (**db)(sqlpp::select(t_table.id).from(t_table).where(t_table.username_safe == username_safe || t_table.username == username).limit(1u));

			if (!result1.empty())
				return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "This nickname already taken!").c_str());
			result1.pop_front();

			std::string email = body["email"];
			auto result2 = (**db)(sqlpp::select(t_table.email).from(t_table).where(t_table.email == email).limit(1u));
			if (!result2.empty())
				return createResponse(Status::CODE_403, himitsu::createError(Status::CODE_403, "This email already taken!").c_str());
			result2.pop_front();

			std::string password = bcrypt::hash(md5::createMD5(body["password"]));
			(**db)(sqlpp::insert_into(t_table).set(
				t_table.username = username,
				t_table.username_safe = username_safe,
				t_table.country = "XX",
				t_table.email = email,
				t_table.password_md5 = password,
				t_table.register_datetime = himitsu::time_convert::getEpochNow(),
				t_table.privileges = 3
			));

			auto result = (**db)(sqlpp::select(t_table.id).from(t_table).where(t_table.email == email).limit(1u));
			int user_id = result.front().id;
			result.pop_front();

			users_stats us_st{};
			users_stats_relax us_st_r{};
			users_preferences us_prf{};

			(**db)(sqlpp::insert_into(us_st).set(us_st.id = user_id));
			(**db)(sqlpp::insert_into(us_st_r).set(us_st_r.id = user_id));
			(**db)(sqlpp::insert_into(us_prf).set(us_prf.id = user_id));

			tokens tok{};
			std::string token = md5::createMD5(himitsu::utils::genRandomString(25));
			(**db)(sqlpp::insert_into(tok).set(
				tok.user = user_id,
				tok.token = token,
				tok.t_private = true,
				tok.privileges = 0,
				tok.last_updated = himitsu::time_convert::getEpochNow()
			));

			json response;
			response["id"] = user_id;
			auto wait = createResponse(Status::CODE_200, response.dump().c_str());
			wait->putHeader("set-cookie",
				fmt::format(
					"hat={}; Path=/; Domain=himitsu.ml; Max-Age={}; Secure",
					token,
					14 * 24 * 60 * 60 // 14 days
				).c_str()
			);
			return wait;
		}

		return createResponse(Status::CODE_400,
			himitsu::createError(Status::CODE_400, "Bad request").c_str()
		);
	}

};

#include OATPP_CODEGEN_END(ApiController)

#endif