#ifndef controller_auth_controller_hpp_included
#define controller_auth_controller_hpp_included

#include "globals.hpp"
#include "handlers/authorization_handler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "database/tables/users_table.hpp"
#include "database/tables/other_table.hpp"

#include <algorithm>
#include <regex>
#include "utils/hash.hpp"
#include "utils/utils.hpp"

#include "handlers/pool_handler.hpp"

using hash = aru::hash;

#include OATPP_CODEGEN_BEGIN(ApiController)

class auth_controller : public oatpp::web::server::api::ApiController
{
private:
	typedef auth_controller __ControllerType;
	std::shared_ptr<token_authorization_handler> token_auth = std::make_shared<token_authorization_handler>();
	auth_controller(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<auth_controller> create_shared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<auth_controller>(new auth_controller(objectMapper));
	}

	ENDPOINT_ASYNC("POST", "/login", _login_user)
	{
		ENDPOINT_ASYNC_INIT(_login_user);

		json body = nullptr;

		Action act() override
		{
			return request->readBodyToStringAsync().callbackTo(&_login_user::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			return pool_handler::startForResult().callbackTo(&_login_user::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (auth_object->valid)
			{
				const tables::tokens tokens_table{};

				db(sqlpp::update(tokens_table).set(
					tokens_table.last_updated = aru::time_convert::get_epoch_now()
				).where(tokens_table.token == auth_object->token));

				json ok;
				ok["id"] = auth_object->user_id;
				ok["token"] = auth_object->token;

				auto response = controller->createResponse(Status::CODE_200, ok.dump().c_str());

				char cookies[128];
				std::ignore = fmt::format_to(cookies, "hat={}; Path=/; Domain={}; Max-Age={}; Secure; HttpOnly",
					auth_object->token, config::frontend_site, 1209600 /* 2 weeks */);

				response->putHeader("set-cookie", cookies);
				return _return(response);
			}

			if (body.is_discarded() || !body["username"].is_string() || !body["password"].is_string())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			std::string password = body["password"];
			std::string username = aru::utils::str_tolower(body["username"]);

			aru::utils::trim(username);
			std::replace(username.begin(), username.end(), ' ', '_');

			const tables::users users_table{};

			auto result = db(sqlpp::select(
				users_table.id, users_table.password_md5, users_table.salt
			).from(users_table).where(users_table.safe_username == username).limit(1u));

			if (result.empty())
			{
				auto error = aru::create_error(Status::CODE_401, "Wrong login");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			const auto& row = result.front();
			int32_t user_id = row.id;

			if (user_id == 1)
			{
				auto error = aru::create_error(Status::CODE_403, "No.");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			if (hash::create_sha512(hash::create_md5(password), row.salt) != row.password_md5.value())
			{
				auto error = aru::create_error(Status::CODE_401, "Wrong login");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			const tables::tokens tokens_table{};
			std::string token;

			// Delete every old token of this user
			db(sqlpp::remove_from(tokens_table).where(tokens_table.user == user_id and tokens_table._private == true));

			while (true)
			{
				token = hash::create_md5(aru::utils::gen_random_string(25));
				auto result = db(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.token == token).limit(1u));

				if (result.empty())
				{
					db(sqlpp::insert_into(tokens_table).set(
						tokens_table.user = user_id,
						tokens_table.token = token,
						tokens_table._private = true,
						tokens_table.privileges = 0,
						tokens_table.last_updated = aru::time_convert::get_epoch_now()
					));
					break;
				}

				// This token exist, create next one! (We check this because we can have same token as another user, oops...)
				result.pop_front();
			}

			json response;
			response["id"] = user_id;
			response["token"] = token;

			auto wait = controller->createResponse(Status::CODE_200, response.dump().c_str());

			char cookies[128];
			std::ignore = fmt::format_to(cookies, "hat={}; Path=/; Domain={}; Max-Age={}; Secure; HttpOnly",
				token, config::frontend_site, 1209600 /* 2 weeks */);

			wait->putHeader("set-cookie", cookies);
			return _return(wait);
		}
	};

	ENDPOINT_ASYNC("POST", "/register", _register_user)
	{
		ENDPOINT_ASYNC_INIT(_register_user);

		json body = nullptr;

		Action act() override
		{
			return request->readBodyToStringAsync().callbackTo(&_register_user::on_body);
		}

		Action on_body(const oatpp::String & request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			return pool_handler::startForResult().callbackTo(&_register_user::on_database);
		}

		Action on_database(const aru::database & db)
		{
			if (body.is_discarded() || !body["username"].is_string() || !body["password"].is_string() || !body["email"].is_string())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			std::string username = body["username"];
			aru::utils::trim(username);

			std::string safe_username = username;
			aru::utils::str_tolower(safe_username);
			std::replace(safe_username.begin(), safe_username.end(), ' ', '_');

			static std::regex user_regex("^[A-Za-z0-9 _[\\]-]{2,15}$");

			if (!std::regex_search(username, user_regex))
			{
				auto error = aru::create_error(521, "This nickname contains forbidden symbols. Allowed symbols: a-Z 0-9 _[]-");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			if (username.find('_') != std::string::npos && username.find(' ') != std::string::npos)
			{
				auto error = aru::create_error(522, "Nickname should not contain spaces and underscores at the same time.");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			if (std::any_of(
				config::forbidden_nicknames.begin(),
				config::forbidden_nicknames.end(),
				[&](std::string& nickname) { return nickname == safe_username; }
			))
			{
				auto error = aru::create_error(523, "This nickname is forbidden. If you are real owner of this nickname, please contact us.");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			std::string email = body["email"];

			const tables::users users_table{};
			auto nickname_check = db(sqlpp::select(users_table.id).from(users_table).where(users_table.safe_username == safe_username || users_table.username == username).limit(1u));

			if (!nickname_check.empty())
			{
				auto error = aru::create_error(524, "This nickname already taken!");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			nickname_check.pop_front();

			auto email_check = db(sqlpp::select(users_table.email).from(users_table).where(users_table.email == email).limit(1u));

			if (!email_check.empty())
			{
				auto error = aru::create_error(525, "This email already taken!");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			email_check.pop_front();

			std::string salt = aru::utils::gen_random_string(24);
			std::string password = hash::create_sha512(hash::create_md5(body["password"]), salt);

			int32_t user_id = db(sqlpp::insert_into(users_table).set(
				users_table.username = username,
				users_table.safe_username = safe_username,
				users_table.country = "XX",
				users_table.email = email,
				users_table.password_md5 = password,
				users_table.salt = salt,
				users_table.ip = aru::utils::get_ip_address(request),
				users_table.registration_date = aru::time_convert::get_epoch_now(),
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
				token = hash::create_md5(aru::utils::gen_random_string(25));
				auto result = db(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.token == token).limit(1u));
				if (result.empty())
				{
					db(sqlpp::insert_into(tokens_table).set(
						tokens_table.user = user_id,
						tokens_table.token = token,
						tokens_table._private = true,
						tokens_table.privileges = 0,
						tokens_table.last_updated = aru::time_convert::get_epoch_now()
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

			char cookies[128];
			std::ignore = fmt::format_to(cookies, "hat={}; Path=/; Domain={}; Max-Age={}; Secure; HttpOnly",
				token, config::frontend_site, 1209600 /* 2 weeks */);

			wait->putHeader("set-cookie", cookies);
			return _return(wait);
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
