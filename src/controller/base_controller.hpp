#ifndef controller_base_controller_hpp_included
#define controller_base_controller_hpp_included

#include "globals.hpp"
#include "../utils/convert.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "database/tables/users_table.hpp"

#include "handlers/pool_handler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class base_controller : public oatpp::web::server::api::ApiController
{
private:
	typedef base_controller __ControllerType;
	base_controller(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<base_controller> create_shared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<base_controller>(new base_controller(objectMapper));
	}

	ENDPOINT_ASYNC("GET", "/ping", _ping)
	{
		ENDPOINT_ASYNC_INIT(_ping);

		Action act() override
		{
			json response;
			response["message"] = aru::memes[aru::utils::gen_random_number() % aru::memes.size()];
			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("GET", "/leaderboard", _leaderboard)
	{
		ENDPOINT_ASYNC_INIT(_leaderboard);

		int32_t user_mode = 0;
		int32_t relax = 0;
		int32_t page = 0;
		int32_t length = 50;

		Action act() override
		{
			user_mode = aru::convert::safe_int(request->getQueryParameter("mode"), 0);
			relax = aru::convert::safe_int(request->getQueryParameter("relax"), 0);
			page = aru::convert::safe_int(request->getQueryParameter("page"), 0);
			length = aru::convert::safe_int(request->getQueryParameter("length"), 50);

			return pool_handler::startForResult().callbackTo(&_leaderboard::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::users users_table{};
			json response = json::array();

			std::pair<uint32_t, uint32_t> limit = sql_helper::paginate(page, length, 100);
			if (relax == 1)
			{
				const tables::users_stats_relax users_stats_table{};
				auto query = sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username, users_table.country)
					.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
					.offset(limit.first).limit(limit.second).unconditionally();

				auto result = db(query);

				for (const auto& row : result)
				{
					switch (user_mode)
					{
						default:
						case 0:
						{
							int64_t position = row.rank_std;
							if (position == 0)
							{
								continue;
							}


							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_std.value();
							user["play_count"] = row.play_count_std.value();
							user["accuracy"] = row.avg_accuracy_std.value();
							user["pp"] = row.pp_std.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
						case 1:
						{
							int64_t position = row.rank_taiko;
							if (position == 0)
							{
								continue;
							}

							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_taiko.value();
							user["play_count"] = row.play_count_taiko.value();
							user["accuracy"] = row.avg_accuracy_taiko.value();
							user["pp"] = row.pp_taiko.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
						case 2:
						{
							int64_t position = row.rank_ctb;
							if (position == 0)
							{
								continue;
							}

							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_ctb.value();
							user["play_count"] = row.play_count_ctb.value();
							user["accuracy"] = row.avg_accuracy_ctb.value();
							user["pp"] = row.pp_ctb.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
						case 3:
						{
							return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
						}
					}
				}
			}
			else
			{
				const tables::users_stats users_stats_table{};
				auto result = db(sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username, users_table.country)
					.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
					.offset(limit.first).limit(limit.second).unconditionally());

				for (const auto& row : result)
				{
					switch (user_mode)
					{
						default:
						case 0:
						{
							int64_t position = row.rank_std;
							if (position == 0)
							{
								continue;
							}

							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_std.value();
							user["play_count"] = row.play_count_std.value();
							user["accuracy"] = row.avg_accuracy_std.value();
							user["pp"] = row.pp_std.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
						case 1:
						{
							int64_t position = row.rank_taiko;
							if (position == 0)
							{
								continue;
							}

							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_taiko.value();
							user["play_count"] = row.play_count_taiko.value();
							user["accuracy"] = row.avg_accuracy_taiko.value();
							user["pp"] = row.pp_taiko.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
						case 2:
						{
							int64_t position = row.rank_ctb;
							if (position == 0)
							{
								continue;
							}

							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_ctb.value();
							user["play_count"] = row.play_count_ctb.value();
							user["accuracy"] = row.avg_accuracy_ctb.value();
							user["pp"] = row.pp_ctb.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
						case 3:
						{
							int64_t position = row.rank_mania;
							if (position == 0)
							{
								continue;
							}

							json user;

							user["id"] = row.id.value();
							user["username"] = row.username.value();
							user["country"] = row.country.value();
							user["ranked_score"] = row.ranked_score_mania.value();
							user["play_count"] = row.play_count_mania.value();
							user["accuracy"] = row.avg_accuracy_mania.value();
							user["pp"] = row.pp_mania.value();
							user["global_rank"] = position;

							response.push_back(user);
							break;
						}
					}
				}
			}

			std::sort(response.begin(), response.end(), [](json first, json second) { return first["global_rank"] < second["global_rank"]; });
			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
