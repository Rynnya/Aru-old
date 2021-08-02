#ifndef controller_BeatmapController_hpp_included
#define controller_BeatmapController_hpp_included

#include "Globals.hpp"
#include "utils/convert.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "handlers/AuthorizationHandler.hpp"

#include "database/tables/BeatmapTable.hpp"
#include "database/tables/ScoresTable.hpp"
#include "database/tables/UsersTable.hpp"
#include "database/tables/OtherTable.hpp"

#include "handlers/PoolHandler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class BeatmapController : public oatpp::web::server::api::ApiController
{
private:
	typedef BeatmapController __ControllerType;
	std::shared_ptr<TokenAuthorizationHandler> tokenAuth = std::make_shared<TokenAuthorizationHandler>();
	void logRankChange(const aru::Connection& db, int32_t id, int64_t bid, int32_t status, std::string type) const;
	BeatmapController(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<BeatmapController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<BeatmapController>(new BeatmapController(objectMapper));
	}

	ENDPOINT_ASYNC("GET", "/beatmapset/{id}", beatmapSet)
	{
		ENDPOINT_ASYNC_INIT(beatmapSet);

		int32_t beatmapset_id = -1;

		Action act() override
		{
			beatmapset_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (beatmapset_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request (id is not a number)")));

			return PoolHandler::startForResult().callbackTo(&beatmapSet::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			const tables::beatmaps beatmaps_table{};
			auto result = db(sqlpp::select(all_of(beatmaps_table)).from(beatmaps_table).where(beatmaps_table.beatmapset_id == beatmapset_id));

			if (result.empty())
				return _return(controller->createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Cannot find beatmap set")));

			json response = json::array();
			for (const auto& row : result)
			{
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
				beatmap["ranked_status"] = row.ranked_status.value();
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
				response.push_back(beatmap);
			}

			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("GET", "/beatmap/{id}", beatmapDefault)
	{
		ENDPOINT_ASYNC_INIT(beatmapDefault);

		int32_t beatmap_id = -1;

		Action act() override
		{
			beatmap_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (beatmap_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request (id is not a number)")));

			return PoolHandler::startForResult().callbackTo(&beatmapDefault::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			const tables::beatmaps beatmaps_table{};
			auto result = db(sqlpp::select(all_of(beatmaps_table)).from(beatmaps_table).where(beatmaps_table.beatmap_id == beatmap_id));

			if (result.empty())
				return _return(controller->createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Cannot find beatmap")));

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

			return _return(controller->createResponse(Status::CODE_200, beatmap.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("GET", "/beatmap/{id}/leaderboard", beatmapLeaderboard)
	{
		ENDPOINT_ASYNC_INIT(beatmapLeaderboard);

		int32_t beatmap_id = -1;
		int32_t mode = -1;
		int32_t relax = 0;
		int32_t length = 50;

		Action act() override
		{
			beatmap_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (beatmap_id == -1)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request (id is not a number)")));

			mode = aru::convert::safe_int(request->getPathVariable("mode"), -1);
			relax = aru::convert::safe_int(request->getPathVariable("relax"), 0);
			length = aru::convert::safe_int(request->getPathVariable("length"), 50);

			return PoolHandler::startForResult().callbackTo(&beatmapLeaderboard::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			const tables::beatmaps b_table{};
			bool isRelax = aru::utils::intToBoolean(relax);
			int32_t play_mode = mode;
			if (play_mode == 3 && isRelax)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Mania don't have relax mode")));

			std::string md5 = "";
			auto res = db(sqlpp::select(b_table.mode, b_table.beatmap_md5).from(b_table).where(b_table.beatmap_id == beatmap_id).limit(1u));

			if (res.empty())
				return _return(controller->createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Cannot find beatmap")));

			const auto& r = res.front();
			play_mode = play_mode == -1 ? r.mode : play_mode;
			md5 = r.beatmap_md5;

			const tables::users users_table{};
			const tables::scores scores_table{};

			auto query = sqlpp::select(scores_table.user_id, users_table.username, users_table.country, scores_table.ranking,
				scores_table.score, scores_table.pp, scores_table.accuracy, scores_table.max_combo, scores_table.mods,
				scores_table.count_300, scores_table.count_100, scores_table.count_50, scores_table.count_misses)
				.from(scores_table.join(users_table).on(scores_table.user_id == users_table.id))
				.where(scores_table.beatmap_md5 == md5 and scores_table.is_relax == isRelax and scores_table.play_mode == play_mode and scores_table.completed);

			std::pair<uint32_t, uint32_t> limit = SQLHelper::Paginate(1, length, 100);
			auto result = db(query.offset(limit.first).limit(limit.second));

			json response = json::array();
			for (const auto& row : result)
			{
				json score;
				score["user_id"] = row.user_id.value();
				score["rank"] = row.ranking.value();
				score["username"] = row.username.value();
				score["country"] = row.country.value();
				score["score"] = row.score.value();
				score["pp"] = row.pp.value();
				score["accuracy"] = row.accuracy.value();
				score["count_300"] = row.count_300.value();
				score["count_100"] = row.count_100.value();
				score["count_50"] = row.count_50.value();
				score["count_miss"] = row.count_misses.value();
				score["max_combo"] = row.max_combo.value();
				score["mods"] = row.mods.value();
				response.push_back(score);
			}

			std::sort(response.begin(), response.end(), [](json first, json second) { return first["pp"] < second["pp"]; });
			return _return(controller->createResponse(Status::CODE_200, response.dump().c_str()));
		}
	};

	ENDPOINT_ASYNC("POST", "/beatmap/ranking", ranking)
	{
		ENDPOINT_ASYNC_INIT(ranking);

		json beatmap = nullptr;

		Action act() override
		{
			return request->readBodyToStringAsync().callbackTo(&ranking::onBody);
		}

		Action onBody(const oatpp::String & request_body)
		{
			beatmap = json::parse(request_body->c_str(), nullptr, false);
			return PoolHandler::startForResult().callbackTo(&ranking::onDatabase);
		}

		Action onDatabase(const aru::Connection& db)
		{
			auto authObject = controller->tokenAuth->handleAuthorization(db, request->getHeader("Authorization"));

			if (!authObject->valid)
				return _return(controller->createResponse(Status::CODE_401, aru::createError(Status::CODE_401, "Unauthorized")));

			if (!beatmap["id"].is_number() || authObject->userID != beatmap["id"].get<int64_t>())
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Forbidden")));

			int32_t id = authObject->userID;
			const tables::users users_table{};
			auto result = db(sqlpp::select(users_table.roles).from(users_table).where(users_table.id == id));

			auto& role = result.front();
			if ((role.roles & 256) == 0)
				return _return(controller->createResponse(Status::CODE_403, aru::createError(Status::CODE_403, "Insufficient permissions")));

			if (!beatmap["bid"].is_number() && !beatmap["status"].is_number() && !beatmap["type"].is_string())
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			const tables::beatmaps beatmaps_tables{};
			int64_t bid = beatmap["bid"].get<int64_t>();
			int32_t status = beatmap["status"].get<int32_t>();
			std::string type = beatmap["type"].get<std::string>();

			if (status < -2 || status > 5)
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			if (type == "b")
			{
				auto& exist = db(sqlpp::select(beatmaps_tables.beatmap_md5).from(beatmaps_tables).where(beatmaps_tables.beatmap_id == bid));
				if (exist.empty())
					return _return(controller->createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Beatmap not found")));

				db(sqlpp::update(beatmaps_tables).set(beatmaps_tables.ranked_status = status).where(beatmaps_tables.beatmap_id == bid));
				controller->logRankChange(db, id, bid, status, type);
			}
			else if (type == "s")
			{
				auto& exist = db(sqlpp::select(beatmaps_tables.beatmap_md5).from(beatmaps_tables).where(beatmaps_tables.beatmapset_id == bid));
				if (exist.empty())
					return _return(controller->createResponse(Status::CODE_404, aru::createError(Status::CODE_404, "Beatmapset not found")));

				db(sqlpp::update(beatmaps_tables).set(beatmaps_tables.ranked_status = status).where(beatmaps_tables.beatmapset_id == bid));
				controller->logRankChange(db, id, bid, status, type);
			}
			else
				return _return(controller->createResponse(Status::CODE_400, aru::createError(Status::CODE_400, "Bad request")));

			return _return(controller->createResponse(Status::CODE_200, ""));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
