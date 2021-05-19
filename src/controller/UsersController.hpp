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

class UsersController : public oatpp::web::server::api::ApiController {
private:
	typedef UsersController __ControllerType;
public:
	UsersController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}
private:
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

	ENDPOINT("GET", "/users/{id}", userInfo, PATH(Int32, id), QUERY(Int32, user_mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"))
	{
		OATPP_COMPONENT(std::shared_ptr<himitsu::redis>, m_redis);

		std::string mode;
		if (user_mode == -1)
		{
			if (!getMode(id, &mode))
			{
				return createResponse(Status::CODE_404,
					himitsu::createError(Status::CODE_404, "player not found").c_str()
				);
			}
		}
		else
			mode = himitsu::osu::modeToString(user_mode);

		if (relax == 1 && mode == "mania")
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "mania don't have relax mode").c_str()
			);
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();

		users user_data{};
		json response;

		users_stats table{};
		if (relax == 1)
		{
			users_stats_relax table{};
			auto query = sqlpp::dynamic_select(**db).dynamic_columns(
				user_data.id, user_data.username, user_data.country, user_data.status, user_data.favourite_mode, user_data.favourite_relax
			).from(user_data.join(table).on(user_data.id == table.id)).where(user_data.is_public == true and user_data.id == (*id)).limit(1u);

			switch (himitsu::osu::modeToInt(mode))
			{
				default:
				{
					query.selected_columns.add(table.pp_std);
					query.selected_columns.add(table.avg_accuracy_std);
					query.selected_columns.add(table.playcount_std);
					break;
				}
				case 1:
				{
					query.selected_columns.add(table.pp_taiko);
					query.selected_columns.add(table.avg_accuracy_taiko);
					query.selected_columns.add(table.playcount_taiko);
					break;
				}
				case 2:
				{
					query.selected_columns.add(table.pp_ctb);
					query.selected_columns.add(table.avg_accuracy_ctb);
					query.selected_columns.add(table.playcount_ctb);
					break;
				}
			}

			auto result = (**db)(query);
			if (result.empty())
			{
				return createResponse(Status::CODE_404,
					himitsu::createError(Status::CODE_404, "player not found").c_str()
				);
			}

			const auto& row = result.front();

			std::string country = row.country;
			country = himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["country"] = row.country.value();
			response["status"] = row.status.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = row.favourite_relax.value();
			response["pp"] = row.at(fmt::format("pp_{0}", mode)).value();
			response["accuracy"] = row.at(fmt::format("avg_accuracy_{0}", mode)).value();
			response["playcount"] = row.at(fmt::format("playcount_{0}", mode)).value();
			response["country_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}:{1}{2}", mode, country, relax == 1 ? ":relax" : "")));
		}
		else
		{
			users_stats table{};
			auto query = sqlpp::dynamic_select(**db).dynamic_columns(
				user_data.id, user_data.username, user_data.country, user_data.status, user_data.favourite_mode, user_data.favourite_relax
			).from(user_data.join(table).on(user_data.id == table.id)).where(user_data.is_public == true and user_data.id == (*id)).limit(1u);

			switch (himitsu::osu::modeToInt(mode))
			{
				default:
				{
					query.selected_columns.add(table.pp_std);
					query.selected_columns.add(table.avg_accuracy_std);
					query.selected_columns.add(table.playcount_std);
					break;
				}
				case 1:
				{
					query.selected_columns.add(table.pp_taiko);
					query.selected_columns.add(table.avg_accuracy_taiko);
					query.selected_columns.add(table.playcount_taiko);
					break;
				}
				case 2:
				{
					query.selected_columns.add(table.pp_ctb);
					query.selected_columns.add(table.avg_accuracy_ctb);
					query.selected_columns.add(table.playcount_ctb);
					break;
				}
				case 3:
				{
					query.selected_columns.add(table.pp_mania);
					query.selected_columns.add(table.avg_accuracy_mania);
					query.selected_columns.add(table.playcount_mania);
					break;
				}
			}

			auto result = (**db)(query);
			if (result.empty())
			{
				return createResponse(Status::CODE_404,
					himitsu::createError(Status::CODE_404, "player not found").c_str()
				);
			}

			const auto& row = result.front();

			std::string country = row.country;
			country = himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["country"] = row.country.value();
			response["status"] = row.status.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = row.favourite_relax.value();
			response["pp"] = row.at(fmt::format("pp_{0}", mode)).value();
			response["accuracy"] = row.at(fmt::format("avg_accuracy_{0}", mode)).value();
			response["playcount"] = row.at(fmt::format("playcount_{0}", mode)).value();
			response["country_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}:{1}{2}", mode, country, relax == 1 ? ":relax" : "")));
		}

		response["global_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : "")));

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/users/{id}/full", fullUserInfo, PATH(Int32, id), QUERY(Int32, user_mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"))
	{
		std::string mode;
		if (user_mode == -1)
		{
			if (!getMode(id, &mode))
			{
				return createResponse(Status::CODE_404,
					himitsu::createError(Status::CODE_404, "player not found").c_str()
				);
			}
		}
		else
			mode = himitsu::osu::modeToString(user_mode);

		if (relax == 1 && mode == "mania")
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "mania don't have relax mode").c_str()
			);
		}

		OATPP_COMPONENT(std::shared_ptr<himitsu::redis>, m_redis);
		auto db = himitsu::ConnectionPool::getInstance()->getConnection();

		users user_data{};
		json response;

		if (relax == 1)
		{
			users_stats_relax table{};
			auto result = (**db)(sqlpp::select( // id inside users_stats_relax
				user_data.username, user_data.register_datetime, user_data.latest_activity,
				user_data.country, user_data.status, user_data.favourite_mode, user_data.favourite_relax, user_data.play_style,
				sqlpp::all_of(table)
			).from(user_data.join(table).on(user_data.id == table.id)).where(user_data.is_public == true and user_data.id == (*id)).limit(1u));

			if (result.empty())
			{
				return createResponse(Status::CODE_404,
					himitsu::createError(Status::CODE_404, "player not found").c_str()
				);
			}

			const auto& row = result.front();

			std::string country = row.country;
			country = himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["register_time"] = himitsu::time_convert::getDate(row.register_datetime);
			response["latest_activity"] = himitsu::time_convert::getDate(row.latest_activity);
			response["status"] = row.status.value();
			response["country"] = row.country.value();
			response["play_style"] = row.play_style.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = row.favourite_relax.value();

			response["stats"]["global_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : "")));
			response["stats"]["country_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}:{1}{2}", mode, country, relax == 1 ? ":relax" : "")));

			long long score = 0;
			switch (himitsu::osu::modeToInt(mode))
			{
				default:
					score = row.total_score_std;
					response["stats"]["ranked_score"] = row.ranked_score_std.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_std.value();
					response["stats"]["total_hits"] = row.total_hits_std.value();
					response["stats"]["accuracy"] = row.avg_accuracy_std.value();
					response["stats"]["pp"] = row.pp_std.value();
					response["stats"]["playtime"] = row.playtime_std.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_std.value();
					response["stats"]["count_S"] = row.count_S_std.value();
					response["stats"]["count_SH"] = row.count_SH_std.value();
					response["stats"]["count_X"] = row.count_X_std.value();
					response["stats"]["count_XH"] = row.count_XH_std.value();
					break;
				case 1:
					score = row.total_score_taiko;
					response["stats"]["ranked_score"] = row.ranked_score_taiko.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_taiko.value();
					response["stats"]["total_hits"] = row.total_hits_taiko.value();
					response["stats"]["accuracy"] = row.avg_accuracy_taiko.value();
					response["stats"]["pp"] = row.pp_taiko.value();
					response["stats"]["playtime"] = row.playtime_taiko.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_taiko.value();
					response["stats"]["count_S"] = row.count_S_taiko.value();
					response["stats"]["count_SH"] = row.count_SH_taiko.value();
					response["stats"]["count_X"] = row.count_X_taiko.value();
					response["stats"]["count_XH"] = row.count_XH_taiko.value();
					break;
				case 2:
					score = row.total_score_ctb;
					response["stats"]["ranked_score"] = row.ranked_score_ctb.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_ctb.value();
					response["stats"]["total_hits"] = row.total_hits_ctb.value();
					response["stats"]["accuracy"] = row.avg_accuracy_ctb.value();
					response["stats"]["pp"] = row.pp_ctb.value();
					response["stats"]["playtime"] = row.playtime_ctb.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_ctb.value();
					response["stats"]["count_S"] = row.count_S_ctb.value();
					response["stats"]["count_SH"] = row.count_SH_ctb.value();
					response["stats"]["count_X"] = row.count_X_ctb.value();
					response["stats"]["count_XH"] = row.count_XH_ctb.value();
					break;
			}
		}
		else
		{
			users_stats table{};
			auto result = (**db)(sqlpp::select( // id inside users_stats
				user_data.username, user_data.register_datetime, user_data.latest_activity,
				user_data.country, user_data.status, user_data.favourite_mode, user_data.favourite_relax, user_data.play_style,
				sqlpp::all_of(table)
			).from(user_data.join(table).on(user_data.id == table.id)).where(user_data.is_public == true and user_data.id == (*id)).limit(1u));

			if (result.empty())
			{
				return createResponse(Status::CODE_404,
					himitsu::createError(Status::CODE_404, "player not found").c_str()
				);
			}

			const auto& row = result.front();

			std::string country = row.country;
			country = himitsu::utils::str_tolower(country);

			response["id"] = row.id.value();
			response["username"] = row.username.value();
			response["register_time"] = himitsu::time_convert::getDate(row.register_datetime);
			response["latest_activity"] = himitsu::time_convert::getDate(row.latest_activity);
			response["status"] = row.status.value();
			response["country"] = row.country.value();
			response["play_style"] = row.play_style.value();
			response["default_mode"] = row.favourite_mode.value();
			response["default_relax"] = row.favourite_relax.value();

			response["stats"]["global_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : "")));
			response["stats"]["country_rank"] = std::stoi(m_redis->getRedisString(fmt::format("ripple:leaderboard:{0}:{1}{2}", mode, country, relax == 1 ? ":relax" : "")));

			long long score = 0;
			switch (himitsu::osu::modeToInt(mode))
			{
				default:
					score = row.total_score_std;
					response["stats"]["ranked_score"] = row.ranked_score_std.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_std.value();
					response["stats"]["total_hits"] = row.total_hits_std.value();
					response["stats"]["accuracy"] = row.avg_accuracy_std.value();
					response["stats"]["pp"] = row.pp_std.value();
					response["stats"]["playtime"] = row.playtime_std.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_std.value();
					response["stats"]["count_S"] = row.count_S_std.value();
					response["stats"]["count_SH"] = row.count_SH_std.value();
					response["stats"]["count_X"] = row.count_X_std.value();
					response["stats"]["count_XH"] = row.count_XH_std.value();
					break;
				case 1:
					score = row.total_score_taiko;
					response["stats"]["ranked_score"] = row.ranked_score_taiko.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_taiko.value();
					response["stats"]["total_hits"] = row.total_hits_taiko.value();
					response["stats"]["accuracy"] = row.avg_accuracy_taiko.value();
					response["stats"]["pp"] = row.pp_taiko.value();
					response["stats"]["playtime"] = row.playtime_taiko.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_taiko.value();
					response["stats"]["count_S"] = row.count_S_taiko.value();
					response["stats"]["count_SH"] = row.count_SH_taiko.value();
					response["stats"]["count_X"] = row.count_X_taiko.value();
					response["stats"]["count_XH"] = row.count_XH_taiko.value();
					break;
				case 2:
					score = row.total_score_ctb;
					response["stats"]["ranked_score"] = row.ranked_score_ctb.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_ctb.value();
					response["stats"]["total_hits"] = row.total_hits_ctb.value();
					response["stats"]["accuracy"] = row.avg_accuracy_ctb.value();
					response["stats"]["pp"] = row.pp_ctb.value();
					response["stats"]["playtime"] = row.playtime_ctb.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_ctb.value();
					response["stats"]["count_S"] = row.count_S_ctb.value();
					response["stats"]["count_SH"] = row.count_SH_ctb.value();
					response["stats"]["count_X"] = row.count_X_ctb.value();
					response["stats"]["count_XH"] = row.count_XH_ctb.value();
					break;
				case 3:
					score = row.total_score_mania;
					response["stats"]["ranked_score"] = row.ranked_score_mania.value();
					response["stats"]["total_score"] = score;
					response["stats"]["playcount"] = row.playcount_mania.value();
					response["stats"]["total_hits"] = row.total_hits_mania.value();
					response["stats"]["accuracy"] = row.avg_accuracy_mania.value();
					response["stats"]["pp"] = row.pp_mania.value();
					response["stats"]["playtime"] = row.playtime_mania.value();
					response["stats"]["level"] = himitsu::osu_level::GetLevel(score);
					response["stats"]["percentage"] = himitsu::osu_level::GetLevelPrecise(score);
					response["stats"]["count_A"] = row.count_A_mania.value();
					response["stats"]["count_S"] = row.count_S_mania.value();
					response["stats"]["count_SH"] = row.count_SH_mania.value();
					response["stats"]["count_X"] = row.count_X_mania.value();
					response["stats"]["count_XH"] = row.count_XH_mania.value();
					break;
			}
		}

		user_badges badges{};
		response["badges"] = json::array();
		for (const auto& badge : (**db)(sqlpp::select(badges.badge).from(badges).where(badges.id == (*id))))
			response["badges"].push_back(badge.badge.value());

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/users/{id}/profile", userProfile, PATH(Int32, id))
	{
		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user_data{};

		auto result = (**db)(sqlpp::select(user_data.background, user_data.userpage).from(user_data).where(user_data.id == (*id)).limit(1u));
		if (result.empty())
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "player not found").c_str()
			);
		}

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