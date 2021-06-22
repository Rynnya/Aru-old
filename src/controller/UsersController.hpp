#ifndef controller_UsersController_hpp_included
#define controller_UsersController_hpp_included

#include "Globals.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "database/tables/UsersTable.hpp"

SQLPP_ALIAS_PROVIDER(score_t);
SQLPP_ALIAS_PROVIDER(beatmap_t);

#include OATPP_CODEGEN_BEGIN(ApiController)

class UsersController : public oatpp::web::server::api::ApiController
{
private:
	typedef UsersController __ControllerType;
	enum class scores_type
	{
		Best = 0,
		Recent = 1,
		First = 2
	};
	std::shared_ptr<UsersController::OutgoingResponse> buildScores(Int32 id, Int32 mode, Int32 relax, Int32 page, Int32 length, scores_type type) const;
	// also checks if player exists. if not, returns false, otherwise returns true and mode as string in ans
	bool getMode(Int32 id, std::string* ans) const;
public:
	UsersController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}

	ENDPOINT("GET", "/users/{id}", userInfo, PATH(Int32, id), QUERY(Int32, user_mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"))
	{
		if (relax == 1 && user_mode == 3)
			return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Mania don't have relax mode").c_str());

		auto db(himitsu::ConnectionPool::getInstance()->getConnection());

		const tables::users users_table{};
		json response;

		if (relax == 1)
		{
			const tables::users_stats_relax users_stats_table{};
			auto query = sqlpp::select(sqlpp::all_of(users_stats_table), users_table.username, 
				users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax)
				.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
				.where(users_table.is_public == true and users_table.id == (*id)).limit(1u);

			auto result = (*db)(query);
			if (result.empty())
				return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Player not found").c_str());

			const auto& row = result.front();

			std::string country = row.country;
			himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["country"] = row.country.value();
			response["status"] = row.status.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = (int)row.favourite_relax;

			switch (user_mode)
			{
				default:
				{
					response["global_rank"] = row.rank_std.value();
					response["pp"] = row.pp_std.value();
					response["accuracy"] = row.avg_accuracy_std.value();
					response["playcount"] = row.playcount_std.value();
					break;
				}
				case 1:
				{
					response["global_rank"] = row.rank_taiko.value();
					response["pp"] = row.pp_taiko.value();
					response["accuracy"] = row.avg_accuracy_taiko.value();
					response["playcount"] = row.playcount_taiko.value();
					break;
				}
				case 2:
				{
					response["global_rank"] = row.rank_ctb.value();
					response["pp"] = row.pp_ctb.value();
					response["accuracy"] = row.avg_accuracy_ctb.value();
					response["playcount"] = row.playcount_ctb.value();
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
				.where(users_table.is_public == true and users_table.id == (*id)).limit(1u);

			auto result = (*db)(query);
			if (result.empty())
				return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Player not found").c_str());

			const auto& row = result.front();

			std::string country = row.country;
			himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["country"] = row.country.value();
			response["status"] = row.status.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = (int)row.favourite_relax;

			switch (user_mode)
			{
				default:
				{
					response["global_rank"] = row.rank_std.value();
					response["pp"] = row.pp_std.value();
					response["accuracy"] = row.avg_accuracy_std.value();
					response["playcount"] = row.playcount_std.value();
					break;
				}

				case 1:
				{
					response["global_rank"] = row.rank_taiko.value();
					response["pp"] = row.pp_taiko.value();
					response["accuracy"] = row.avg_accuracy_taiko.value();
					response["playcount"] = row.playcount_taiko.value();
					break;
				}

				case 2:
				{
					response["global_rank"] = row.rank_ctb.value();
					response["pp"] = row.pp_ctb.value();
					response["accuracy"] = row.avg_accuracy_ctb.value();
					response["playcount"] = row.playcount_ctb.value();
					break;
				}

				case 3:
				{
					response["global_rank"] = row.rank_mania.value();
					response["pp"] = row.pp_mania.value();
					response["accuracy"] = row.avg_accuracy_mania.value();
					response["playcount"] = row.playcount_mania.value();
					break;
				}
			}
		}

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/users/{id}/full", fullUserInfo, PATH(Int32, id), QUERY(Int32, user_mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"))
	{

		if (relax == 1 && user_mode == 3)
			return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Mania don't have relax mode").c_str());

		auto db(himitsu::ConnectionPool::getInstance()->getConnection());

		const tables::users users_table{};
		json response;

		if (relax == 1)
		{
			const tables::users_stats_relax users_stats_table{};
			auto result = (*db)(sqlpp::select( // id inside users_stats_relax
				users_table.username, users_table.registration_date, users_table.latest_activity,
				users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax, users_table.play_style,
				sqlpp::all_of(users_stats_table)
			).from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id)).where(users_table.is_public == true and users_table.id == (*id)).limit(1u));

			if (result.empty())
				return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Player not found").c_str());

			const auto& row = result.front();

			std::string country = row.country;
			himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["register_time"] = himitsu::time_convert::getDate(row.registration_date);
			response["latest_activity"] = himitsu::time_convert::getDate(row.latest_activity);
			response["status"] = row.status.value();
			response["country"] = row.country.value();
			response["playstyle"] = row.play_style.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = (int)row.favourite_relax;

			long long score = 0;
			switch (user_mode)
			{
				default:
				{
					score = row.total_score_std;
					response["stats"]["global_rank"] = row.rank_std.value();
					response["stats"]["ranked_score"] = row.ranked_score_std.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_std.value();
					response["stats"]["total_hits"] = row.total_hits_std.value();
					response["stats"]["accuracy"] = row.avg_accuracy_std.value();
					response["stats"]["pp"] = row.pp_std.value();
					response["stats"]["playtime"] = row.playtime_std.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
					response["stats"]["playcount"] = row.playcount_taiko.value();
					response["stats"]["total_hits"] = row.total_hits_taiko.value();
					response["stats"]["accuracy"] = row.avg_accuracy_taiko.value();
					response["stats"]["pp"] = row.pp_taiko.value();
					response["stats"]["playtime"] = row.playtime_taiko.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
					response["stats"]["playcount"] = row.playcount_ctb.value();
					response["stats"]["total_hits"] = row.total_hits_ctb.value();
					response["stats"]["accuracy"] = row.avg_accuracy_ctb.value();
					response["stats"]["pp"] = row.pp_ctb.value();
					response["stats"]["playtime"] = row.playtime_ctb.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
			auto result = (*db)(sqlpp::select( // id inside users_stats
				users_table.username, users_table.registration_date, users_table.latest_activity,
				users_table.country, users_table.status, users_table.favourite_mode, users_table.favourite_relax, users_table.play_style,
				sqlpp::all_of(users_stats_table)
			).from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id)).where(users_table.is_public == true and users_table.id == (*id)).limit(1u));

			if (result.empty())
				return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Player not found").c_str());

			const auto& row = result.front();

			std::string country = row.country;
			himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["register_time"] = himitsu::time_convert::getDate(row.registration_date);
			response["latest_activity"] = himitsu::time_convert::getDate(row.latest_activity);
			response["status"] = row.status.value();
			response["country"] = row.country.value();
			response["playstyle"] = row.play_style.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = (int)row.favourite_relax;

			long long score = 0;
			switch (user_mode)
			{
				default:
				{
					score = row.total_score_std;
					response["stats"]["global_rank"] = row.rank_std.value();
					response["stats"]["ranked_score"] = row.ranked_score_std.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_std.value();
					response["stats"]["total_hits"] = row.total_hits_std.value();
					response["stats"]["accuracy"] = row.avg_accuracy_std.value();
					response["stats"]["pp"] = row.pp_std.value();
					response["stats"]["playtime"] = row.playtime_std.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
					response["stats"]["playcount"] = row.playcount_taiko.value();
					response["stats"]["total_hits"] = row.total_hits_taiko.value();
					response["stats"]["accuracy"] = row.avg_accuracy_taiko.value();
					response["stats"]["pp"] = row.pp_taiko.value();
					response["stats"]["playtime"] = row.playtime_taiko.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
					response["stats"]["playcount"] = row.playcount_ctb.value();
					response["stats"]["total_hits"] = row.total_hits_ctb.value();
					response["stats"]["accuracy"] = row.avg_accuracy_ctb.value();
					response["stats"]["pp"] = row.pp_ctb.value();
					response["stats"]["playtime"] = row.playtime_ctb.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
					response["stats"]["playcount"] = row.playcount_mania.value();
					response["stats"]["total_hits"] = row.total_hits_mania.value();
					response["stats"]["accuracy"] = row.avg_accuracy_mania.value();
					response["stats"]["pp"] = row.pp_mania.value();
					response["stats"]["playtime"] = row.playtime_mania.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevelPrecise(score);
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
		for (const auto& badge : (*db)(sqlpp::select(badges_table.badge).from(badges_table).where(badges_table.id == (*id))))
			response["badges"].push_back(badge.badge.value());

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/users/{id}/profile", userProfile, PATH(Int32, id))
	{
		auto db(himitsu::ConnectionPool::getInstance()->getConnection());
		const tables::users users_table{};

		auto result = (*db)(sqlpp::select(users_table.background, users_table.userpage).from(users_table).where(users_table.id == (*id)).limit(1u));
		if (result.empty())
			return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Player not found").c_str());

		json response;
		const auto& row = result.front();

		response["background"] = row.background.value();
		response["userpage"] = row.userpage.value();

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/users/{id}/scores/best", bestScores,
		PATH(Int32, id), QUERY(Int32, mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, page, "page", "0"), QUERY(Int32, length, "length", "50"))
	{
		return buildScores(id, mode, relax, page, length, scores_type::Best);
	};

	ENDPOINT("GET", "/users/{id}/scores/recent", recentScores,
		PATH(Int32, id), QUERY(Int32, mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, page, "page", "0"), QUERY(Int32, length, "length", "50"))
	{
		return buildScores(id, mode, relax, page, length, scores_type::Recent);
	};

	ENDPOINT("GET", "/users/{id}/scores/first", firstScores,
		PATH(Int32, id), QUERY(Int32, mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, page, "page", "0"), QUERY(Int32, length, "length", "50"))
	{
		return buildScores(id, mode, relax, page, length, scores_type::First);
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif