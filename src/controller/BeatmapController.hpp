#ifndef controller_BeatmapController_hpp_included
#define controller_BeatmapController_hpp_included

#include "Globals.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "handlers/AuthorizationHandler.hpp"

#include "database/tables/BeatmapTable.hpp"
#include "database/tables/ScoresTable.hpp"
#include "database/tables/UsersTable.hpp"

#include <thread>

#include OATPP_CODEGEN_BEGIN(ApiController)

class BeatmapController : public oatpp::web::server::api::ApiController
{
private:
	typedef BeatmapController __ControllerType;
	std::shared_ptr<TokenAuthorizationHandler> tokenAuth = std::make_shared<TokenAuthorizationHandler>();
	void logRankChange(aru::Connection db, int32_t id, int64_t bid, int32_t status, std::string type);
public:
	BeatmapController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}

	ENDPOINT("GET", "/beatmapset/{id}", beatmapSet, PATH(Int32, id))
	{
		const tables::beatmaps beatmaps_table{};
		auto db(aru::ConnectionPool::getInstance()->getConnection());
		auto result = db(sqlpp::select(all_of(beatmaps_table)).from(beatmaps_table).where(beatmaps_table.beatmapset_id == (*id)));

		if (result.empty())
			return createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Cannot find beatmap set").c_str());

		json response = json::array();
		for (const auto& row : result)
		{
			json beatmap;
			beatmap["beatmap_id"]    = row.beatmap_id.value();
			beatmap["beatmapset_id"] = row.beatmapset_id.value();
			beatmap["artist"]        = row.artist.value();
			beatmap["title"]         = row.title.value();
			beatmap["version"]       = row.difficulty_name.value();
			beatmap["creator"]       = row.creator.value();
			beatmap["count_normal"]  = row.count_normal.value();
			beatmap["count_slider"]  = row.count_slider.value();
			beatmap["count_spinner"] = row.count_spinner.value();
			beatmap["max_combo"]     = row.max_combo.value();
			beatmap["ranked_status"] = row.ranked_status.value();
			beatmap["creating_date"] = row.creating_date.value();
			beatmap["bpm"]           = row.bpm.value();
			beatmap["hit_length"]    = row.hit_length.value();

			switch (row.mode)
			{
				default:
					beatmap["difficulty"] = row.difficulty_std.value();
					break;
				case 1:
					beatmap["difficulty"] = row.difficulty_taiko.value();
					break;
				case 2:
					beatmap["difficulty"] = row.difficulty_ctb.value();
					break;
				case 3:
					beatmap["difficulty"] = row.difficulty_mania.value();
					break;
			}

			json diff;
			diff["std"]   = row.difficulty_std.value();
			diff["taiko"] = row.difficulty_taiko.value();
			diff["ctb"]   = row.difficulty_ctb.value();
			diff["mania"] = row.difficulty_mania.value();
			beatmap["difficulties"] = diff;

			beatmap["cs"] = row.cs.value();
			beatmap["ar"] = row.ar.value();
			beatmap["od"] = row.od.value();
			beatmap["hp"] = row.hp.value();

			beatmap["mode"] = row.mode.value();
			response.push_back(beatmap);
		}

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/beatmap/{id}", beatmapDefault, PATH(Int32, id))
	{
		const tables::beatmaps beatmaps_table{};
		auto db(aru::ConnectionPool::getInstance()->getConnection());
		auto result = db(sqlpp::select(all_of(beatmaps_table)).from(beatmaps_table).where(beatmaps_table.beatmap_id == (*id)));

		if (result.empty())
			return createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Cannot find beatmap").c_str());

		const auto& row = result.front();
		json beatmap;

		beatmap["beatmap_id"] = row.beatmap_id.value();
		beatmap["beatmapset_id"] = row.beatmapset_id.value();
		beatmap["artist"] = row.artist.value();
		beatmap["title"] = row.title.value();
		beatmap["version"] = row.difficulty_name.value();
		beatmap["creator"] = row.creator.value();
		beatmap["count_normal"] = row.count_normal.value();
		beatmap["count_slider"] = row.count_slider.value();
		beatmap["count_spinner"] = row.count_spinner.value();
		beatmap["max_combo"] = row.max_combo.value();
		beatmap["ranked"] = row.ranked_status.value();
		beatmap["creating_date"] = row.creating_date.value();
		beatmap["bpm"] = row.bpm.value();
		beatmap["hit_length"] = row.hit_length.value();

		switch (row.mode)
		{
			default:
				beatmap["difficulty"] = row.difficulty_std.value();
				break;
			case 1:
				beatmap["difficulty"] = row.difficulty_taiko.value();
				break;
			case 2:
				beatmap["difficulty"] = row.difficulty_ctb.value();
				break;
			case 3:
				beatmap["difficulty"] = row.difficulty_mania.value();
				break;
		}

		json diff;
		diff["std"] = row.difficulty_std.value();
		diff["taiko"] = row.difficulty_taiko.value();
		diff["ctb"] = row.difficulty_ctb.value();
		diff["mania"] = row.difficulty_mania.value();
		beatmap["difficulties"] = diff;

		beatmap["cs"] = row.cs.value();
		beatmap["ar"] = row.ar.value();
		beatmap["od"] = row.od.value();
		beatmap["hp"] = row.hp.value();

		beatmap["mode"] = row.mode.value();

		return createResponse(Status::CODE_200, beatmap.dump().c_str());
	};

	ENDPOINT("GET", "/beatmap/{id}/leaderboard", beatmapLeaderboard, 
		PATH(Int32, id), QUERY(Int32, mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, length, "length", "50"))
	{
		const tables::beatmaps b_table{};
		auto db(aru::ConnectionPool::getInstance()->getConnection());

		bool isRelax = aru::utils::intToBoolean(relax);
		int32_t play_mode = mode;
		if (play_mode == 3 && isRelax)
			return createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Mania don't have relax mode").c_str());

		std::string md5 = "";
		auto res = db(sqlpp::select(b_table.mode, b_table.beatmap_md5)
			.from(b_table)
			.where(b_table.beatmap_id == (*id))
			.limit(1u)
		);
		if (res.empty())
			return createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Cannot find beatmap").c_str());

		const auto& r = res.front();
		if (play_mode == -1) play_mode = r.mode;
		md5 = r.beatmap_md5;

		const tables::users users_table{};
		const tables::scores scores_table{};

		auto query = sqlpp::select(scores_table.user_id, users_table.username, users_table.country, scores_table.ranking,
			scores_table.score, scores_table.pp, scores_table.accuracy, scores_table.max_combo, scores_table.mods,
			scores_table.count_300, scores_table.count_100, scores_table.count_50, scores_table.count_misses)
			.from(scores_table.join(users_table).on(scores_table.user_id == users_table.id))
			.where(scores_table.beatmap_md5 == md5 and scores_table.is_relax == isRelax and scores_table.play_mode == play_mode and scores_table.completed)
			.order_by(scores_table.pp.desc());
		std::pair<uint32_t, uint32_t> limit = SQLHelper::Paginate(1, length, 100);
		auto result = db(query.offset(limit.first).limit(limit.second));

		json response = json::array();
		for (const auto& row : result)
		{
			json score;
			score["user_id"]      = row.user_id.value();
			score["rank"]         = row.ranking.value();
 			score["username"]     = row.username.value();
			score["country"]      = row.country.value();
			score["score"]        = row.score.value();
			score["pp"]           = row.pp.value();
			score["accuracy"]     = row.accuracy.value();
			score["count_300"]    = row.count_300.value();
			score["count_100"]    = row.count_100.value();
			score["count_50"]     = row.count_50.value();
			score["count_miss"]   = row.count_misses.value();
			score["max_combo"]    = row.max_combo.value();
			score["mods"]         = row.mods.value();
			response.push_back(score);
		}

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("POST", "/beatmap/ranking", ranking,
		AUTHORIZATION(std::shared_ptr<TokenObject>, authObject, tokenAuth), BODY_STRING(String, userRequest))
	{
		if (!authObject->valid)
			return createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized").c_str());

		json beatmap = json::parse(userRequest->c_str(), nullptr, false);
		if (!beatmap["id"].is_number() || authObject->userID != beatmap["id"].get<int64_t>())
			return createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden").c_str());

		int32_t userID = authObject->userID;
		auto db(aru::ConnectionPool::getInstance()->getConnection());
		const tables::users users_table {};
		auto result = db(sqlpp::select(users_table.roles).from(users_table).where(users_table.id == userID));

		auto& role = result.front();
		if (role.roles & (1 << 8))
		{
			if (beatmap["bid"].is_number() && beatmap["status"].is_number() && beatmap["type"].is_string())
			{
				const tables::beatmaps beatmaps_tables {};
				int64_t bid = beatmap["bid"].get<int64_t>();
				int32_t status = beatmap["status"].get<int32_t>();
				std::string type = beatmap["type"].get<std::string>();

				if (status < -2 || status > 5)
					return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());

				if (type == "b")
				{
					auto& exist = db(sqlpp::select(beatmaps_tables.beatmap_md5).from(beatmaps_tables).where(beatmaps_tables.beatmap_id == bid));
					if (exist.empty())
						return createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Beatmap not found").c_str());

					db(sqlpp::update(beatmaps_tables).set(beatmaps_tables.ranked_status = status).where(beatmaps_tables.beatmap_id == bid));
					logRankChange(std::move(db), userID, bid, status, type);
				}
				else if (type == "s")
				{
					auto& exist = db(sqlpp::select(beatmaps_tables.beatmap_md5).from(beatmaps_tables).where(beatmaps_tables.beatmapset_id == bid));
					if (exist.empty())
						return createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Beatmapset not found").c_str());

					db(sqlpp::update(beatmaps_tables).set(beatmaps_tables.ranked_status = status).where(beatmaps_tables.beatmapset_id == bid));
					logRankChange(std::move(db), userID, bid, status, type);
				}
				else
					return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());

				return createResponse(Status::CODE_200, "");
			}

			return createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request").c_str());
		}

		return createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Insufficient permissions").c_str());
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif