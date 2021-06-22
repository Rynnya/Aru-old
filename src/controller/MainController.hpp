#ifndef controller_MainController_hpp_included
#define controller_MainController_hpp_included

#include "Globals.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "database/tables/UsersTable.hpp"

#include "cpp_redis/misc/error.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class MainController : public oatpp::web::server::api::ApiController
{
private:
	typedef MainController __ControllerType;
public:
	MainController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}

	ENDPOINT("GET", "/ping", Ping, REQUEST(std::shared_ptr<IncomingRequest>, request))
	{
		json response;
		response["message"] = himitsu::memes[himitsu::utils::genRandomInt() % himitsu::memes.size()].c_str();
		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/leaderboard", Leaderboard, 
		QUERY(Int32, user_mode, "mode", "0"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, page, "page", "0"), QUERY(Int32, length, "length", "50"))
	{
		std::string mode = himitsu::osu::modeToString(user_mode);

		const tables::users users_table{};
		auto db(himitsu::ConnectionPool::getInstance()->getConnection());
		json response = json::array();

		if (relax == 1)
		{
			const tables::users_stats_relax users_stats_table{};
			auto query = sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username, users_table.country)
				.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
				.unconditionally();

			auto result = (*db)(query);

			for (const auto& row : result)
			{
				switch (user_mode)
				{
					default:
					{
						int64_t position = row.rank_std;
						if (position == 0)
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_std.value();
						user["playcount"] = row.playcount_std.value();
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
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_taiko.value();
						user["playcount"] = row.playcount_taiko.value();
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
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_ctb.value();
						user["playcount"] = row.playcount_ctb.value();
						user["accuracy"] = row.avg_accuracy_ctb.value();
						user["pp"] = row.pp_ctb.value();
						user["global_rank"] = position;

						response.push_back(user);
						break;
					}
				}
			}
		}
		else
		{
			const tables::users_stats users_stats_table{};
			auto query = sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username, users_table.country)
				.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
				.unconditionally();

			auto result = (*db)(query);

			for (const auto& row : result)
			{
				switch (user_mode)
				{
					default:
					{
						int64_t position = row.rank_std;
						if (position == 0)
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_std.value();
						user["playcount"] = row.playcount_std.value();
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
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_taiko.value();
						user["playcount"] = row.playcount_taiko.value();
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
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_ctb.value();
						user["playcount"] = row.playcount_ctb.value();
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
							continue;

						json user;

						user["id"] = row.id.value();
						user["username"] = row.username.value();
						user["country"] = row.country.value();
						user["ranked_score"] = row.ranked_score_mania.value();
						user["playcount"] = row.playcount_mania.value();
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
		return createResponse(Status::CODE_200, response.dump().c_str());
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
