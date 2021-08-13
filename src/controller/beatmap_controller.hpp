#ifndef controller_beatmap_controller_hpp_included
#define controller_beatmap_controller_hpp_included

#include "globals.hpp"
#include "utils/convert.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "handlers/authorization_handler.hpp"

#include "database/tables/beatmap_table.hpp"
#include "database/tables/scores_table.hpp"
#include "database/tables/users_table.hpp"
#include "database/tables/other_table.hpp"

#include "handlers/pool_handler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class beatmap_controller : public oatpp::web::server::api::ApiController
{
private:
	typedef beatmap_controller __ControllerType;
	std::shared_ptr<token_authorization_handler> token_auth = std::make_shared<token_authorization_handler>();
	void log_rank_change(const aru::database& db, int32_t id, int64_t bid, int32_t status, std::string type) const;
	beatmap_controller(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper) {}
public:

	static std::shared_ptr<beatmap_controller> create_shared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::shared_ptr<beatmap_controller>(new beatmap_controller(objectMapper));
	}

	ENDPOINT_ASYNC("GET", "/beatmapset/{id}", _beatmap_set)
	{
		ENDPOINT_ASYNC_INIT(_beatmap_set);

		int32_t beatmapset_id = -1;

		Action act() override
		{
			beatmapset_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (beatmapset_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_beatmap_set::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::beatmaps beatmaps_table{};
			auto result = db(sqlpp::select(all_of(beatmaps_table)).from(beatmaps_table).where(beatmaps_table.beatmapset_id == beatmapset_id));

			if (result.empty())
			{
				auto error = aru::create_error(Status::CODE_404, "Cannot find beatmap set");
				return _return(controller->createResponse(Status::CODE_404, error));
			}
			
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
					case 0:
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

	ENDPOINT_ASYNC("GET", "/beatmap/{id}", _beatmap_default)
	{
		ENDPOINT_ASYNC_INIT(_beatmap_default);

		int32_t beatmap_id = -1;

		Action act() override
		{
			beatmap_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (beatmap_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return pool_handler::startForResult().callbackTo(&_beatmap_default::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::beatmaps beatmaps_table{};
			auto result = db(sqlpp::select(all_of(beatmaps_table)).from(beatmaps_table).where(beatmaps_table.beatmap_id == beatmap_id));

			if (result.empty())
			{
				auto error = aru::create_error(Status::CODE_404, "Cannot find beatmap");
				return _return(controller->createResponse(Status::CODE_404, error));
			}

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
				case 0:
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

	ENDPOINT_ASYNC("GET", "/beatmap/{id}/leaderboard", _beatmap_leaderboard)
	{
		ENDPOINT_ASYNC_INIT(_beatmap_leaderboard);

		int32_t beatmap_id = -1;
		int32_t mode = -1;
		int32_t relax = 0;
		int32_t length = 50;

		Action act() override
		{
			beatmap_id = aru::convert::safe_int(request->getPathVariable("id"), -1);
			if (beatmap_id == -1)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request (id is not a number)");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			mode = aru::convert::safe_int(request->getPathVariable("mode"), -1);
			relax = aru::convert::safe_int(request->getPathVariable("relax"), 0);
			length = aru::convert::safe_int(request->getPathVariable("length"), 50);

			return pool_handler::startForResult().callbackTo(&_beatmap_leaderboard::on_database);
		}

		Action on_database(const aru::database& db)
		{
			const tables::beatmaps b_table{};
			bool is_relax = aru::utils::int_to_bool(relax);
			if (mode == 3 && is_relax)
			{
				auto error = aru::create_error(Status::CODE_400, "Mania don't have relax mode");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			std::string md5 = "";
			auto res = db(sqlpp::select(b_table.mode, b_table.beatmap_md5).from(b_table).where(b_table.beatmap_id == beatmap_id).limit(1u));

			if (res.empty())
			{
				auto error = aru::create_error(Status::CODE_404, "Cannot find beatmap");
				return _return(controller->createResponse(Status::CODE_404, error));
			}

			const auto& r = res.front();
			mode == -1 ? r.mode : mode;
			md5 = r.beatmap_md5;

			const tables::users users_table{};
			const tables::scores scores_table{};

			auto query = sqlpp::select(scores_table.user_id, users_table.username, users_table.country, scores_table.ranking,
				scores_table.score, scores_table.pp, scores_table.accuracy, scores_table.max_combo, scores_table.mods,
				scores_table.count_300, scores_table.count_100, scores_table.count_50, scores_table.count_misses)
				.from(scores_table.join(users_table).on(scores_table.user_id == users_table.id))
				.where(scores_table.beatmap_md5 == md5 and scores_table.is_relax == is_relax and scores_table.play_mode == mode and scores_table.completed);

			std::pair<uint32_t, uint32_t> limit = sql_helper::paginate(1, length, 100);
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

	ENDPOINT_ASYNC("POST", "/beatmap/ranking", _ranking)
	{
		ENDPOINT_ASYNC_INIT(_ranking);

		json beatmap = nullptr;

		Action act() override
		{
			return request->readBodyToStringAsync().callbackTo(&_ranking::on_body);
		}

		Action on_body(const oatpp::String & request_body)
		{
			beatmap = json::parse(request_body->c_str(), nullptr, false);
			return pool_handler::startForResult().callbackTo(&_ranking::on_database);
		}

		Action on_database(const aru::database& db)
		{
			std::shared_ptr<token_object> auth_object = controller->token_auth->handle_authorization(db, request);

			if (!auth_object->valid)
			{
				auto error = aru::create_error(Status::CODE_401, "Unauthorized");
				return _return(controller->createResponse(Status::CODE_401, error));
			}

			if (!beatmap["id"].is_number() || auth_object->user_id != beatmap["id"].get<int64_t>())
			{
				auto error = aru::create_error(Status::CODE_403, "Forbidden");
				return _return(controller->createResponse(Status::CODE_403, error));
			}

			int32_t id = auth_object->user_id;
			const tables::users users_table{};
			auto result = db(sqlpp::select(users_table.roles).from(users_table).where(users_table.id == id));

			auto& role = result.front();
			if ((role.roles & 256) == 0)
			{
				auto error = aru::create_error(Status::CODE_403, "Insufficient permissions");
				return _return(controller->createResponse(Status::CODE_403, error));
			}
			
			if (!beatmap["bid"].is_number() && !beatmap["status"].is_number() && !beatmap["type"].is_string())
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			const tables::beatmaps beatmaps_tables{};
			int64_t bid = beatmap["bid"].get<int64_t>();
			int32_t status = beatmap["status"].get<int32_t>();
			std::string type = beatmap["type"].get<std::string>();

			if (status < -2 || status > 5)
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			if (type == "b")
			{
				auto& exist = db(sqlpp::select(beatmaps_tables.beatmap_md5).from(beatmaps_tables).where(beatmaps_tables.beatmap_id == bid));
				if (exist.empty())
				{
					auto error = aru::create_error(Status::CODE_404, "Beatmap not found");
					return _return(controller->createResponse(Status::CODE_404, error));
				}

				db(sqlpp::update(beatmaps_tables).set(beatmaps_tables.ranked_status = status).where(beatmaps_tables.beatmap_id == bid));
				controller->log_rank_change(db, id, bid, status, type);
			}
			else if (type == "s")
			{
				auto& exist = db(sqlpp::select(beatmaps_tables.beatmap_md5).from(beatmaps_tables).where(beatmaps_tables.beatmapset_id == bid));
				if (exist.empty())
				{
					auto error = aru::create_error(Status::CODE_404, "Beatmapset not found");
					return _return(controller->createResponse(Status::CODE_404, error));
				}

				db(sqlpp::update(beatmaps_tables).set(beatmaps_tables.ranked_status = status).where(beatmaps_tables.beatmapset_id == bid));
				controller->log_rank_change(db, id, bid, status, type);
			}
			else
			{
				auto error = aru::create_error(Status::CODE_400, "Bad request");
				return _return(controller->createResponse(Status::CODE_400, error));
			}

			return _return(controller->createResponse(Status::CODE_200, ""));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)

#endif
