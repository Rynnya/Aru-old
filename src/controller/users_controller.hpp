#ifndef controller_users_controller_hpp_included
#define controller_users_controller_hpp_included

#include "globals.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "database/tables/users_table.hpp"
#include "utils/convert.hpp"

#include "handlers/pool_handler.hpp"

SQLPP_ALIAS_PROVIDER(score_t);
SQLPP_ALIAS_PROVIDER(beatmap_t);

#include OATPP_CODEGEN_BEGIN(ApiController)

class users_controller : public oatpp::web::server::api::ApiController
{
private:
	typedef users_controller __ControllerType;
	enum class scores_type
	{
		best = 0,
		recent = 1,
		first = 2
	};
	std::shared_ptr<users_controller::OutgoingResponse> build_scores(const aru::database& db, int32_t id, int32_t mode, int32_t relax, int32_t page, int32_t length, scores_type type) const;
	// also checks if player exists. if not, returns false, otherwise returns true and mode as string in ans
	int32_t get_mode(const aru::database& db, int32_t id) const;
	users_controller(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<users_controller> create_shared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<users_controller>(new users_controller(objectMapper));
	}

	ENDPOINT_ASYNC("GET", "/users/{id}", _user_info)
	{
		ENDPOINT_ASYNC_INIT(_user_info);

		int32_t user_id = -1;
		int32_t user_mode = -1;
		int32_t relax = 0;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			user_mode = aru::convert::safe_int(request->getQueryParameter("mode"), -1);
			relax = aru::convert::safe_int(request->getQueryParameter("relax"), 0);

			if (relax == 1 && user_mode == 3)
			{
				auto error = aru::create_error(Status::CODE_400, "Mania don't have relax mode");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_user_info::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::users users_table{};
			json response;

			if (relax == 1)
			{
				const tables::users_stats_relax users_stats_table{};
				auto result = db(sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username,
					users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax)
					.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
					.where(users_table.is_public == true and users_table.id == user_id).limit(1u));

				if (result.empty())
				{
					auto error = aru::create_error(Status::CODE_404, "Player not found");
					return _return(controller->createResponse(Status::CODE_404, error));
				}

				const auto& row = result.front();

				std::string country = row.country;
				aru::utils::str_tolower(country);

				response["id"] = row.id.value();
				response["username"] = row.username.value();
				response["country"] = row.country.value();
				response["status"] = row.status.value();
				response["default_mode"] = row.favourite_mode.value();
				response["default_relax"] = static_cast<int32_t>(row.favourite_relax);

				switch (user_mode)
				{
					default:
					case 0:
					{
						response["global_rank"] = row.rank_std.value();
						response["pp"] = row.pp_std.value();
						response["accuracy"] = row.avg_accuracy_std.value();
						response["play_count"] = row.play_count_std.value();
						break;
					}
					case 1:
					{
						response["global_rank"] = row.rank_taiko.value();
						response["pp"] = row.pp_taiko.value();
						response["accuracy"] = row.avg_accuracy_taiko.value();
						response["play_count"] = row.play_count_taiko.value();
						break;
					}
					case 2:
					{
						response["global_rank"] = row.rank_ctb.value();
						response["pp"] = row.pp_ctb.value();
						response["accuracy"] = row.avg_accuracy_ctb.value();
						response["play_count"] = row.play_count_ctb.value();
						break;
					}
				}
			}
			else
			{
				const tables::users_stats users_stats_table{};
				auto query = sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username,
					users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax)
					.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
					.where(users_table.is_public == true and users_table.id == user_id).limit(1u);

				auto result = db(query);
				if (result.empty())
				{
					auto error = aru::create_error(Status::CODE_404, "Player not found");
					return _return(controller->createResponse(Status::CODE_404, error));
				}

				const auto& row = result.front();

				std::string country = row.country;
				aru::utils::str_tolower(country);

				response["id"] = row.id.value();
				response["username"] = row.username.value();
				response["country"] = row.country.value();
				response["status"] = row.status.value();
				response["default_mode"] = row.favourite_mode.value();
				response["default_relax"] = static_cast<int32_t>(row.favourite_relax);

				switch (user_mode)
				{
					default:
					case 0:
					{
						response["global_rank"] = row.rank_std.value();
						response["pp"] = row.pp_std.value();
						response["accuracy"] = row.avg_accuracy_std.value();
						response["play_count"] = row.play_count_std.value();
						break;
					}

					case 1:
					{
						response["global_rank"] = row.rank_taiko.value();
						response["pp"] = row.pp_taiko.value();
						response["accuracy"] = row.avg_accuracy_taiko.value();
						response["play_count"] = row.play_count_taiko.value();
						break;
					}

					case 2:
					{
						response["global_rank"] = row.rank_ctb.value();
						response["pp"] = row.pp_ctb.value();
						response["accuracy"] = row.avg_accuracy_ctb.value();
						response["play_count"] = row.play_count_ctb.value();
						break;
					}

					case 3:
					{
						response["global_rank"] = row.rank_mania.value();
						response["pp"] = row.pp_mania.value();
						response["accuracy"] = row.avg_accuracy_mania.value();
						response["play_count"] = row.play_count_mania.value();
						break;
					}
				}
			}

			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("GET", "/users/{id}/full", _full_user_info)
	{
		ENDPOINT_ASYNC_INIT(_full_user_info);

		int32_t user_id = -1;
		int32_t user_mode = -1;
		int32_t relax = 0;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			user_mode = aru::convert::safe_int(request->getQueryParameter("mode"), -1);
			relax = aru::convert::safe_int(request->getQueryParameter("relax"), 0);

			if (relax == 1 && user_mode == 3)
			{
				auto error = aru::create_error(Status::CODE_400, "Mania don't have relax mode");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_full_user_info::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::users users_table{};
			json response;

			if (relax == 1)
			{
				const tables::users_stats_relax users_stats_table{};
				auto result = db(sqlpp::select( // id inside users_stats_relax
					users_table.username, users_table.registration_date, users_table.latest_activity,
					users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax, users_table.play_style,
					sqlpp::all_of(users_stats_table)
				).from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id)
				).where(users_table.is_public == true and users_table.id == user_id).limit(1u));

				if (result.empty())
				{
					auto error = aru::create_error(Status::CODE_404, "Player not found");
					return _return(controller->createResponse(Status::CODE_404, error));
				}

				const auto& row = result.front();

				std::string country = row.country;
				aru::utils::str_tolower(country);

				response["id"] = row.id.value();
				response["username"] = row.username.value();
				response["register_time"] = aru::time_convert::get_date(row.registration_date);
				response["latest_activity"] = aru::time_convert::get_date(row.latest_activity);
				response["status"] = row.status.value();
				response["country"] = row.country.value();
				response["play_style"] = row.play_style.value();
				response["default_mode"] = row.favourite_mode.value();
				response["default_relax"] = (int32_t)row.favourite_relax;

				int64_t score = 0;
				switch (user_mode)
				{
					default:
					{
						score = row.total_score_std;
						response["stats"]["global_rank"] = row.rank_std.value();
						response["stats"]["ranked_score"] = row.ranked_score_std.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_std.value();
						response["stats"]["total_hits"] = row.total_hits_std.value();
						response["stats"]["accuracy"] = row.avg_accuracy_std.value();
						response["stats"]["pp"] = row.pp_std.value();
						response["stats"]["play_time"] = row.play_time_std.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_std.value();
						response["stats"]["count_S"] = row.count_S_std.value();
						response["stats"]["count_SH"] = row.count_SH_std.value();
						response["stats"]["count_X"] = row.count_X_std.value();
						response["stats"]["count_XH"] = row.count_XH_std.value();
						break;
					}

					case 1:
					{
						score = row.total_score_taiko;
						response["stats"]["global_rank"] = row.rank_taiko.value();
						response["stats"]["ranked_score"] = row.ranked_score_taiko.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_taiko.value();
						response["stats"]["total_hits"] = row.total_hits_taiko.value();
						response["stats"]["accuracy"] = row.avg_accuracy_taiko.value();
						response["stats"]["pp"] = row.pp_taiko.value();
						response["stats"]["play_time"] = row.play_time_taiko.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_taiko.value();
						response["stats"]["count_S"] = row.count_S_taiko.value();
						response["stats"]["count_SH"] = row.count_SH_taiko.value();
						response["stats"]["count_X"] = row.count_X_taiko.value();
						response["stats"]["count_XH"] = row.count_XH_taiko.value();
						break;
					}

					case 2:
					{
						score = row.total_score_ctb;
						response["stats"]["global_rank"] = row.rank_ctb.value();
						response["stats"]["ranked_score"] = row.ranked_score_ctb.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_ctb.value();
						response["stats"]["total_hits"] = row.total_hits_ctb.value();
						response["stats"]["accuracy"] = row.avg_accuracy_ctb.value();
						response["stats"]["pp"] = row.pp_ctb.value();
						response["stats"]["play_time"] = row.play_time_ctb.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_ctb.value();
						response["stats"]["count_S"] = row.count_S_ctb.value();
						response["stats"]["count_SH"] = row.count_SH_ctb.value();
						response["stats"]["count_X"] = row.count_X_ctb.value();
						response["stats"]["count_XH"] = row.count_XH_ctb.value();
						break;
					}
				}
			}
			else
			{
				const tables::users_stats users_stats_table{};
				auto result = db(sqlpp::select( // id inside users_stats
					users_table.username, users_table.registration_date, users_table.latest_activity,
					users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax,
					users_table.play_style, sqlpp::all_of(users_stats_table)
				).from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id)
				).where(users_table.is_public == true and users_table.id == user_id).limit(1u));

				if (result.empty())
				{
					auto error = aru::create_error(Status::CODE_404, "Player not found");
					return _return(controller->createResponse(Status::CODE_404, error));
				}

				const auto& row = result.front();

				std::string country = row.country;
				aru::utils::str_tolower(country);

				response["id"] = row.id.value();
				response["username"] = row.username.value();
				response["register_time"] = aru::time_convert::get_date(row.registration_date);
				response["latest_activity"] = aru::time_convert::get_date(row.latest_activity);
				response["status"] = row.status.value();
				response["country"] = row.country.value();
				response["play_style"] = row.play_style.value();
				response["default_mode"] = row.favourite_mode.value();
				response["default_relax"] = (int32_t)row.favourite_relax;

				int64_t score = 0;
				switch (user_mode)
				{
					default:
					{
						score = row.total_score_std;
						response["stats"]["global_rank"] = row.rank_std.value();
						response["stats"]["ranked_score"] = row.ranked_score_std.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_std.value();
						response["stats"]["total_hits"] = row.total_hits_std.value();
						response["stats"]["accuracy"] = row.avg_accuracy_std.value();
						response["stats"]["pp"] = row.pp_std.value();
						response["stats"]["play_time"] = row.play_time_std.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_std.value();
						response["stats"]["count_S"] = row.count_S_std.value();
						response["stats"]["count_SH"] = row.count_SH_std.value();
						response["stats"]["count_X"] = row.count_X_std.value();
						response["stats"]["count_XH"] = row.count_XH_std.value();
						break;
					}

					case 1:
					{
						score = row.total_score_taiko;
						response["stats"]["global_rank"] = row.rank_taiko.value();
						response["stats"]["ranked_score"] = row.ranked_score_taiko.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_taiko.value();
						response["stats"]["total_hits"] = row.total_hits_taiko.value();
						response["stats"]["accuracy"] = row.avg_accuracy_taiko.value();
						response["stats"]["pp"] = row.pp_taiko.value();
						response["stats"]["play_time"] = row.play_time_taiko.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_taiko.value();
						response["stats"]["count_S"] = row.count_S_taiko.value();
						response["stats"]["count_SH"] = row.count_SH_taiko.value();
						response["stats"]["count_X"] = row.count_X_taiko.value();
						response["stats"]["count_XH"] = row.count_XH_taiko.value();
						break;
					}

					case 2:
					{
						score = row.total_score_ctb;
						response["stats"]["global_rank"] = row.rank_ctb.value();
						response["stats"]["ranked_score"] = row.ranked_score_ctb.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_ctb.value();
						response["stats"]["total_hits"] = row.total_hits_ctb.value();
						response["stats"]["accuracy"] = row.avg_accuracy_ctb.value();
						response["stats"]["pp"] = row.pp_ctb.value();
						response["stats"]["play_time"] = row.play_time_ctb.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_ctb.value();
						response["stats"]["count_S"] = row.count_S_ctb.value();
						response["stats"]["count_SH"] = row.count_SH_ctb.value();
						response["stats"]["count_X"] = row.count_X_ctb.value();
						response["stats"]["count_XH"] = row.count_XH_ctb.value();
						break;
					}

					case 3:
					{
						score = row.total_score_mania;
						response["stats"]["global_rank"] = row.rank_mania.value();
						response["stats"]["ranked_score"] = row.ranked_score_mania.value();
						response["stats"]["total_score"] = score;
						response["stats"]["play_count"] = row.play_count_mania.value();
						response["stats"]["total_hits"] = row.total_hits_mania.value();
						response["stats"]["accuracy"] = row.avg_accuracy_mania.value();
						response["stats"]["pp"] = row.pp_mania.value();
						response["stats"]["play_time"] = row.play_time_mania.value();
						response["stats"]["level"] = aru::osu_level::get_level_precise(score);
						response["stats"]["count_A"] = row.count_A_mania.value();
						response["stats"]["count_S"] = row.count_S_mania.value();
						response["stats"]["count_SH"] = row.count_SH_mania.value();
						response["stats"]["count_X"] = row.count_X_mania.value();
						response["stats"]["count_XH"] = row.count_XH_mania.value();
						break;
					}
				}
			}

			const tables::user_badges badges_table{};
			response["badges"] = json::array();
			for (const auto& badge : db(sqlpp::select(badges_table.badge).from(badges_table).where(badges_table.id == user_id)))
			{
				response["badges"].push_back(badge.badge.value());
			}

			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("GET", "/users/{id}/profile", _user_profile)
	{
		ENDPOINT_ASYNC_INIT(_user_profile);

		int32_t user_id = -1;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}
			
			return pool_handler::startForResult().callbackTo(&_user_profile::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::users users_table{};
			auto result = db(sqlpp::select(users_table.background, users_table.userpage).from(users_table).where(users_table.id == user_id).limit(1u));
			if (result.empty())
			{
				auto error = aru::create_error(Status::CODE_404, "Player not found");
				return _return(controller->createResponse(Status::CODE_404, error));
			}

			json response;
			const auto& row = result.front();

			response["background"] = row.background.value();
			response["userpage"] = row.userpage.value();

			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("GET", "/users/{id}/scores/best", _best_scores)
	{
		ENDPOINT_ASYNC_INIT(_best_scores);

		int32_t user_id = -1;
		int32_t mode = -1;
		int32_t relax = 0;
		int32_t page = 0;
		int32_t length = 50;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			mode = aru::convert::safe_int(request->getQueryParameter("mode"), -1);
			relax = aru::convert::safe_int(request->getQueryParameter("relax"), 0);
			page = aru::convert::safe_int(request->getQueryParameter("page"), 0);
			length = aru::convert::safe_int(request->getQueryParameter("length"), 50);

			return pool_handler::startForResult().callbackTo(&_best_scores::on_database);
		}

		Action on_database(const aru::database& db)
		{
			return _return(controller->build_scores(db, user_id, mode, relax, page, length, scores_type::best));
		}
	};

	ENDPOINT_ASYNC("GET", "/users/{id}/scores/recent", _recent_scores)
	{
		ENDPOINT_ASYNC_INIT(_recent_scores);

		int32_t user_id = -1;
		int32_t mode = -1;
		int32_t relax = 0;
		int32_t page = 0;
		int32_t length = 50;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			mode = aru::convert::safe_int(request->getQueryParameter("mode"), -1);
			relax = aru::convert::safe_int(request->getQueryParameter("relax"), 0);
			page = aru::convert::safe_int(request->getQueryParameter("page"), 0);
			length = aru::convert::safe_int(request->getQueryParameter("length"), 50);

			return pool_handler::startForResult().callbackTo(&_recent_scores::on_database);
		}

		Action on_database(const aru::database& db)
		{
			return _return(controller->build_scores(db, user_id, mode, relax, page, length, scores_type::recent));
		}
	};

	ENDPOINT_ASYNC("GET", "/users/{id}/scores/first", _first_scores)
	{
		ENDPOINT_ASYNC_INIT(_first_scores);

		int32_t user_id = -1;
		int32_t mode = -1;
		int32_t relax = 0;
		int32_t page = 0;
		int32_t length = 50;

		Action act() override
		{
			user_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (user_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			mode = aru::convert::safe_int(request->getQueryParameter("mode"), -1);
			relax = aru::convert::safe_int(request->getQueryParameter("relax"), 0);
			page = aru::convert::safe_int(request->getQueryParameter("page"), 0);
			length = aru::convert::safe_int(request->getQueryParameter("length"), 50);

			return pool_handler::startForResult().callbackTo(&_first_scores::on_database);
		}

		Action on_database(const aru::database& db)
		{
			return _return(controller->build_scores(db, user_id, mode, relax, page, length, scores_type::first));
		}
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif
