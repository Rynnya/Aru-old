#ifndef controller_settings_controller_hpp_included
#define controller_settings_controller_hpp_included

#include <filesystem>

#include "globals.hpp"
#include "handlers/authorization_handler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/core/data/stream/FileStream.hpp"

#include "handlers/pool_handler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class settings_controller : public oatpp::web::server::api::ApiController
{
private:
	typedef settings_controller __ControllerType;
	std::shared_ptr<token_authorization_handler> token_auth = std::make_shared<token_authorization_handler>();
	inline static std::unordered_set<int32_t> avatar_limiter = {};
	std::shared_ptr<settings_controller::OutgoingResponse> get_settings(const aru::database& db, int32_t id) const;
	std::shared_ptr<settings_controller::OutgoingResponse> update_background(const aru::database& db, int32_t id, std::string request) const;
	std::shared_ptr<settings_controller::OutgoingResponse> update_userpage(const aru::database& db, int32_t id, std::string request) const;
	std::shared_ptr<settings_controller::OutgoingResponse> update_status(const aru::database& db, int32_t id, std::string request) const;
	std::shared_ptr<settings_controller::OutgoingResponse> update_preferences(const aru::database& db, int32_t id, json body) const;
	std::shared_ptr<settings_controller::OutgoingResponse> update_scoreboard(const aru::database& db, int32_t id, json body) const;
	std::shared_ptr<settings_controller::OutgoingResponse> change_password(const aru::database& db, int32_t id, json body) const;
	settings_controller(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<settings_controller> create_shared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<settings_controller>(new settings_controller(objectMapper));
	}

	ENDPOINT_ASYNC("GET", "/users/{id}/settings", _get_user_settings)
	{
		ENDPOINT_ASYNC_INIT(_get_user_settings);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_get_user_settings::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->get_settings(db, user_id));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/background", _change_background)
	{
		ENDPOINT_ASYNC_INIT(_change_background);

		int32_t user_id = -1;
		oatpp::String body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return request->readBodyToStringAsync().callbackTo(&_change_background::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = request_body;
			return pool_handler::startForResult().callbackTo(&_change_background::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->update_background(db, user_id, body->c_str()));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/userpage", _change_userpage)
	{
		ENDPOINT_ASYNC_INIT(_change_userpage);

		int32_t user_id = -1;
		oatpp::String body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return request->readBodyToStringAsync().callbackTo(&_change_userpage::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = request_body;
			return pool_handler::startForResult().callbackTo(&_change_userpage::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->update_userpage(db, user_id, body->c_str()));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/status", _change_status)
	{
		ENDPOINT_ASYNC_INIT(_change_status);

		int32_t user_id = -1;
		oatpp::String body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return request->readBodyToStringAsync().callbackTo(&_change_status::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = request_body;
			return pool_handler::startForResult().callbackTo(&_change_status::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->update_status(db, user_id, body->c_str()));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/avatar", _change_avatar)
	{
		ENDPOINT_ASYNC_INIT(_change_avatar);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_change_avatar::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			if (config::avatar_folder == "/path/to/folder/{}.png")
			{
				fmt::print("Warning! Avatars currently disabled, please change example directory.\n");
				return _return(controller->createResponse(Status::CODE_422, aru::create_error(Status::CODE_422, "Sorry, but avatars currently not available to change!")));
			}

			if (config::limits::enable_rate_limit && controller->avatar_limiter.find(user_id) != controller->avatar_limiter.end())
			{
				fmt::print("Warning: User {} rate limited.", user_id);
				static oatpp::String error = aru::create_error(Status::CODE_429, fmt::format("Rate limit occurred (Avatar limiter)"));
				return _return(controller->createResponse(Status::CODE_429, error));
			}

			std::ignore = controller->avatar_limiter.insert(user_id);
			auto size = request->getHeader("Content-Length");

			if (!size)
			{
				fmt::print("Warning: User {} trying to send image without Content-Length header.", user_id);
				return _return(controller->createResponse(Status::CODE_400, aru::create_error(Status::CODE_400, "Image cannot be sended without Content-Length header")));
			}

			if (aru::convert::safe_int(size, 1048577) > 1048576 /* 1024 * 1024 */)
			{
				fmt::print("Warning: User {} trying to send really huge image (or maybe not image). Size: {}\n", user_id, size->c_str());
				return _return(controller->createResponse(Status::CODE_400, aru::create_error(Status::CODE_400, "Image files cannot be more than 1 MB")));
			}

			std::shared_ptr<oatpp::data::stream::FileOutputStream> file_output_stream = std::make_shared<oatpp::data::stream::FileOutputStream>(fmt::format(config::avatar_folder, user_id).c_str());
			return request->transferBodyToStreamAsync(file_output_stream).next(yieldTo(&_change_avatar::finish));
		}

		Action finish()
		{
			OATPP_COMPONENT(std::shared_ptr<cpp_redis::client>, redis);
			redis->publish("aru.change_avatar", std::to_string(user_id));
			redis->commit();

			return _return(controller->createResponse(Status::CODE_200, "OK"));
		}
	};

	ENDPOINT_ASYNC("DELETE", "/users/{id}/settings/avatar", _delete_avatar)
	{
		ENDPOINT_ASYNC_INIT(_delete_avatar);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_delete_avatar::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			if (config::avatar_folder == "/path/to/folder/{}.png")
			{
				fmt::print("Warning! Avatars currently disabled, please change example directory.\n");
				auto error = aru::create_error(Status::CODE_422, "Sorry, but avatars currently not available to change!");
				return _return(controller->createResponse(Status::CODE_422, error));
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

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/preferences", _change_preferences)
	{
		ENDPOINT_ASYNC_INIT(_change_preferences);

		int32_t user_id = -1;
		json body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return request->readBodyToStringAsync().callbackTo(&_change_preferences::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			if (body.is_discarded())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_change_preferences::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->update_preferences(db, user_id, body));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/scoreboard", _change_scoreboard)
	{
		ENDPOINT_ASYNC_INIT(_change_scoreboard);

		int32_t user_id = -1;
		json body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return request->readBodyToStringAsync().callbackTo(&_change_scoreboard::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			if (body.is_discarded())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_change_scoreboard::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->update_scoreboard(db, user_id, body));
		}
	};

	ENDPOINT_ASYNC("PUT", "/users/{id}/settings/password", _change_password)
	{
		ENDPOINT_ASYNC_INIT(_change_password);

		int32_t user_id = -1;
		json body = nullptr;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return request->readBodyToStringAsync().callbackTo(&_change_password::on_body);
		}

		Action on_body(const oatpp::String& request_body)
		{
			body = json::parse(request_body->c_str(), nullptr, false);
			if (body.is_discarded())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			if (!body["current_password"].is_string() || !body["new_password"].is_string())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_change_password::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (auth_object->user_id != user_id)
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			return _return(controller->change_password(db, user_id, body));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
